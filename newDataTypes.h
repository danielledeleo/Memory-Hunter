#define MAX_MATRIX_SIZE 128

typedef struct {
	signed long x;
	signed long y;
	signed long z;
} Point3DType;

typedef struct {
	int x_size;
	int y_size;

	int x[MAX_MATRIX_SIZE];
	int y[MAX_MATRIX_SIZE];
} MatrixType;

typedef struct {
	unsigned char ipv4addr[4]; // [255].[255].[255].[255]
	float longitude;
	float latitude;
	char  datacentre[MAX_STR];
	char  hostname[MAX_STR];
	char  os[MAX_STR];
	int   bandwidth;
	int   usage_limit;
} ServerEntryType;