/*
Thomas Zeng & Shaun Baron-Furuyama
*/

/*** Creating and destroying ***/

/* Feel free to read the struct's members, but don't write them, except through
the accessors below such as meshSetTriangle, meshSetVertex. */
typedef struct meshMesh meshMesh;
struct meshMesh
{
	int triNum, vertNum, attrDim;
	int *tri;	  /* triNum * 3 ints */
	double *vert; /* vertNum * attrDim doubles */
};

/* Initializes a mesh with enough memory to hold its triangles and vertices.
Does not actually fill in those triangles or vertices with useful data. When
you are finished with the mesh, you must call meshFinalize to deallocate its
backing resources. */
int meshInitialize(meshMesh *mesh, int triNum, int vertNum, int attrDim)
{
	mesh->tri = (int *)malloc(triNum * 3 * sizeof(int) +
							  vertNum * attrDim * sizeof(double));
	if (mesh->tri != NULL)
	{
		mesh->vert = (double *)&(mesh->tri[triNum * 3]);
		mesh->triNum = triNum;
		mesh->vertNum = vertNum;
		mesh->attrDim = attrDim;
	}
	return (mesh->tri == NULL);
}

/* Sets the trith triangle to have vertex indices i, j, k. */
void meshSetTriangle(meshMesh *mesh, int tri, int i, int j, int k)
{
	if (0 <= tri && tri < mesh->triNum)
	{
		mesh->tri[3 * tri] = i;
		mesh->tri[3 * tri + 1] = j;
		mesh->tri[3 * tri + 2] = k;
	}
}

/* Returns a pointer to the trith triangle. For example:
	int *triangle13 = meshGetTrianglePointer(&mesh, 13);
	printf("%d, %d, %d\n", triangle13[0], triangle13[1], triangle13[2]); */
int *meshGetTrianglePointer(const meshMesh *mesh, int tri)
{
	if (0 <= tri && tri < mesh->triNum)
		return &mesh->tri[tri * 3];
	else
		return NULL;
}

/* Sets the vertth vertex to have attributes attr. */
void meshSetVertex(meshMesh *mesh, int vert, const double attr[])
{
	int k;
	if (0 <= vert && vert < mesh->vertNum)
		for (k = 0; k < mesh->attrDim; k += 1)
			mesh->vert[mesh->attrDim * vert + k] = attr[k];
}

/* Returns a pointer to the vertth vertex. For example:
	double *vertex13 = meshGetVertexPointer(&mesh, 13);
	printf("x = %f, y = %f\n", vertex13[0], vertex13[1]); */
double *meshGetVertexPointer(const meshMesh *mesh, int vert)
{
	if (0 <= vert && vert < mesh->vertNum)
		return &mesh->vert[vert * mesh->attrDim];
	else
		return NULL;
}

/* Deallocates the resources backing the mesh. This function must be called
when you are finished using a mesh. */
void meshFinalize(meshMesh *mesh)
{
	free(mesh->tri);
}

/*** Writing and reading files ***/

/* Helper function for meshInitializeFile. */
int meshFileError(
	meshMesh *mesh, FILE *file, const char *cause, const int line)
{
	fprintf(stderr, "error: meshInitializeFile: %s at line %d\n", cause, line);
	fclose(file);
	meshFinalize(mesh);
	return 3;
}

/* Initializes a mesh from a mesh file. The file format is documented at
meshSaveFile. This function does not do as much error checking as one might
like. Use it only on trusted, non-corrupted files, such as ones that you have
recently created using meshSaveFile. Returns 0 on success, non-zero on failure.
Don't forget to invoke meshFinalize when you are done using the mesh. */
int meshInitializeFile(meshMesh *mesh, const char *path)
{
	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		fprintf(stderr, "error: meshInitializeFile: fopen failed\n");
		return 1;
	}
	int year, month, day, triNum, vertNum, attrDim;
	// Future work: Check version.
	if (fscanf(file, "Carleton College CS 311 mesh version %d/%d/%d\n", &year,
			   &month, &day) != 3)
	{
		fprintf(stderr, "error: meshInitializeFile: bad header at line 1\n");
		fclose(file);
		return 1;
	}
	if (fscanf(file, "triNum %d\n", &triNum) != 1)
	{
		fprintf(stderr, "error: meshInitializeFile: bad triNum at line 2\n");
		fclose(file);
		return 2;
	}
	if (fscanf(file, "vertNum %d\n", &vertNum) != 1)
	{
		fprintf(stderr, "error: meshInitializeFile: bad vertNum at line 3\n");
		fclose(file);
		return 3;
	}
	if (fscanf(file, "attrDim %d\n", &attrDim) != 1)
	{
		fprintf(stderr, "error: meshInitializeFile: bad attrDim at line 4\n");
		fclose(file);
		return 4;
	}
	if (meshInitialize(mesh, triNum, vertNum, attrDim) != 0)
	{
		fclose(file);
		return 5;
	}
	int line = 5, *tri, j, check;
	if (fscanf(file, "%d Triangles:\n", &check) != 1 || check != triNum)
		return meshFileError(mesh, file, "bad header", line);
	for (line = 6; line < triNum + 6; line += 1)
	{
		tri = meshGetTrianglePointer(mesh, line - 6);
		if (fscanf(file, "%d %d %d\n", &tri[0], &tri[1], &tri[2]) != 3)
			return meshFileError(mesh, file, "bad triangle", line);
		if (0 > tri[0] || tri[0] >= vertNum || 0 > tri[1] || tri[1] >= vertNum || 0 > tri[2] || tri[2] >= vertNum)
			return meshFileError(mesh, file, "bad index", line);
	}
	double *vert;
	if (fscanf(file, "%d Vertices:\n", &check) != 1 || check != vertNum)
		return meshFileError(mesh, file, "bad header", line);
	for (line = triNum + 7; line < triNum + 7 + vertNum; line += 1)
	{
		vert = meshGetVertexPointer(mesh, line - (triNum + 7));
		for (j = 0; j < attrDim; j += 1)
		{
			if (fscanf(file, "%lf ", &vert[j]) != 1)
				return meshFileError(mesh, file, "bad vertex", line);
		}
		if (fscanf(file, "\n") != 0)
			return meshFileError(mesh, file, "bad vertex", line);
	}
	// Future work: Check EOF.
	fclose(file);
	return 0;
}

/* Saves a mesh to a file in a simple custom format (not any industry
standard). Returns 0 on success, non-zero on failure. The first line is a
comment of the form 'Carleton College CS 311 mesh version YYYY/MM/DD'.

I now describe version 2019/01/15. The second line says 'triNum [triNum]',
where the latter is an integer value. The third and fourth lines do the same
for vertNum and attrDim. The fifth line says '[triNum] Triangles:'. Then there
are triNum lines, each holding three integers between 0 and vertNum - 1
(separated by a space). Then there is a line that says '[vertNum] Vertices:'.
Then there are vertNum lines, each holding attrDim floating-point numbers
(terminated by a space). */
int meshSaveFile(const meshMesh *mesh, const char *path)
{
	FILE *file = fopen(path, "w");
	if (file == NULL)
	{
		fprintf(stderr, "error: meshSaveFile: fopen failed\n");
		return 1;
	}
	fprintf(file, "Carleton College CS 311 mesh version 2019/01/15\n");
	fprintf(file, "triNum %d\n", mesh->triNum);
	fprintf(file, "vertNum %d\n", mesh->vertNum);
	fprintf(file, "attrDim %d\n", mesh->attrDim);
	fprintf(file, "%d Triangles:\n", mesh->triNum);
	int i, j;
	int *tri;
	for (i = 0; i < mesh->triNum; i += 1)
	{
		tri = meshGetTrianglePointer(mesh, i);
		fprintf(file, "%d %d %d\n", tri[0], tri[1], tri[2]);
	}
	fprintf(file, "%d Vertices:\n", mesh->vertNum);
	double *vert;
	for (i = 0; i < mesh->vertNum; i += 1)
	{
		vert = meshGetVertexPointer(mesh, i);
		for (j = 0; j < mesh->attrDim; j += 1)
			fprintf(file, "%f ", vert[j]);
		fprintf(file, "\n");
	}
	fclose(file);
	return 0;
}

// computes homog and then trirenders
void homogRender(const double a[], const double b[], const double c[], depthBuffer *buf, const double viewport[4][4],
				 const shaShading *sha, const double unif[], const texTexture *tex[])
{

	//copy a,b,c to new vectors, as we cannot overwrte them
	double a_c[sha->varyDim], b_c[sha->varyDim],c_c[sha->varyDim];
	vecCopy(sha->varyDim, a, a_c);
	vecCopy(sha->varyDim, b, b_c);
	vecCopy(sha->varyDim, c, c_c);

	double aHomog[4], bHomog[4], cHomog[4];
	mat441Multiply(viewport, a_c, aHomog);
	mat441Multiply(viewport, b_c, bHomog);
	mat441Multiply(viewport, c_c, cHomog);

	vecCopy(4, aHomog, a_c);
	vecCopy(4, bHomog, b_c);
	vecCopy(4, cHomog, c_c);

	vecScale(sha->varyDim, 1 / a_c[3], a_c, a_c);
	vecScale(sha->varyDim, 1 / b_c[3], b_c, b_c);
	vecScale(sha->varyDim, 1 / c_c[3], c_c, c_c);

	triRender(sha, buf, unif, tex, a_c, b_c, c_c);
}

// clips a
void getClip(int dim, double a[], double b[], double ab[])
{
	double t = (a[2] + a[3]) / (a[2] + a[3] - b[2] - b[3]);

	vecSubtract(dim, b, a, ab);
	vecScale(dim, t, ab, ab);
	vecAdd(dim, a, ab, ab);
}

// clips a, and render
void clipOneVertex(double a[], double b[], double c[], depthBuffer *buf, const double viewport[4][4],
				   const shaShading *sha, const double unif[], const texTexture *tex[])
{

	double ab[sha->varyDim], ac[sha->varyDim];

	getClip(sha->varyDim, a, b, ab);
	getClip(sha->varyDim, a, c, ac);

	homogRender(b, ac, ab, buf, viewport, sha, unif, tex);
	homogRender(c, ac, b, buf, viewport, sha, unif, tex);
}

// clip a,c, and render
void clipTwoVertex(double a[], double b[], double c[], depthBuffer *buf, const double viewport[4][4],
				   const shaShading *sha, const double unif[], const texTexture *tex[])
{
	
	double ba[sha->varyDim], bc[sha->varyDim];
	
	getClip(sha->varyDim, a, b, ba);
	getClip(sha->varyDim, c, b, bc);

	homogRender(b, bc, ba, buf, viewport, sha, unif, tex);
}

/*** Rendering ***/

/* Renders the mesh. If the mesh and the shading have differing values for
attrDim, then prints an error message and does not render anything. */
void meshRender(
	const meshMesh *mesh, depthBuffer *buf, const double viewport[4][4],
	const shaShading *sha, const double unif[], const texTexture *tex[])
{

	if (mesh->attrDim != sha->attrDim)
	{
		fprintf(stderr, "mesh attrDim %d does not equal sha attrDim %d", mesh->attrDim, sha->attrDim);
		return;
	}

	for (int tri = 0; tri < mesh->triNum; tri++)
	{
		int *verts = meshGetTrianglePointer(mesh, tri);

		double a[sha->varyDim], b[sha->varyDim], c[sha->varyDim];

		/* uses attributes at each viertices to set the varyings*/
		sha->shadeVertex(sha->unifDim, unif, sha->attrDim, meshGetVertexPointer(mesh, verts[0]), sha->varyDim, a);
		sha->shadeVertex(sha->unifDim, unif, sha->attrDim, meshGetVertexPointer(mesh, verts[1]), sha->varyDim, b);
		sha->shadeVertex(sha->unifDim, unif, sha->attrDim, meshGetVertexPointer(mesh, verts[2]), sha->varyDim, c);

		// varyw 3 varyz 2
		if (a[3] <= 0 || a[3] < -a[2])
		{
			if (b[3] <= 0 || b[3] < -b[2])
			{
				if (c[3] <= 0 || c[3] < -c[2])
				{
					// all three are clipped
					continue;
				}
				else
				{
					// only c is not clipped
					clipTwoVertex(b, c, a, buf, viewport, sha, unif, tex);
					// continue;
				}
			}
			else if (c[3] <= 0 || c[3] < -c[2])
			{
				// only b is not clipped
				clipTwoVertex(a, b, c, buf, viewport, sha, unif, tex);
				// continue;
			}
			else
			{
				// b,c not clipped
				clipOneVertex(a, b, c, buf, viewport, sha, unif, tex);
				// continue;
			}
		}
		else if (b[3] <= 0 || b[3] < -b[2])
		{
			if (c[3] <= 0 || c[3] < -c[2])
			{
				// only a not clipped
				clipTwoVertex(c, a, b, buf, viewport, sha, unif, tex);
				// continue;
			}
			else
			{
				// a,c not clipped
				clipOneVertex(b, c, a, buf, viewport, sha, unif, tex);
				// continue;
			}
		}
		else if (c[3] <= 0 || c[3] < -c[2])
		{
			// a,b not clipped
			clipOneVertex(c, a, b, buf, viewport, sha, unif, tex);
			// continue;
		}
		else
		{
			// all not clipped
			homogRender(a, b, c, buf, viewport, sha, unif, tex);
		}
	}
}