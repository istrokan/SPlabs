#include <stdio.h>
#include <limits.h>
#include <float.h>
int main(void)
{
	int a = sizeof(bool); int b = sizeof(char); int c = sizeof(short); int d = sizeof(int); int i = sizeof(long); int f = sizeof(float); int g = sizeof(double);
	printf("\n\n\t\t+---------+------+------------------------------+\n");
	printf("\t\t|  Type   | Byte |      Range of values         |\n");
	printf("\t\t+---------+------+------------------------------+\n");
	printf("\t\t| boolean | %db   | true-false                   |\n", a);
	printf("\t\t+---------+------+------------------------------+\n");
	printf("\t\t| char    | %db   | %d-%d                     |\n", b, CHAR_MIN, CHAR_MAX);
	printf("\t\t+---------+------+------------------------------+\n");
	printf("\t\t| short   | %db   | %d-%d                 |\n", c, SHRT_MIN, SHRT_MAX);
	printf("\t\t+---------+------+------------------------------+\n");
	printf("\t\t| int     | %db   | %d-%d       |\n", d, INT_MIN, INT_MAX);
	printf("\t\t+---------+------+------------------------------+\n");
	printf("\t\t| long    | %db   | 0-%lu                 |\n", i, ULONG_MAX);
	printf("\t\t+---------+------+------------------------------+\n");
	printf("\t\t| float   | %db   | %e-%e    |\n", f, FLT_MIN, FLT_MAX);
	printf("\t\t+---------+------+------------------------------+\n");
	printf("\t\t| double  | %db   | %e-%e  |\n", g, DBL_MIN, DBL_MAX);
	printf("\t\t+---------+------+------------------------------+\n");
	return 0;
}

