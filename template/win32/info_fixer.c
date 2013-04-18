#include <stdio.h>

int main(int argc, char * argv[])
{
	FILE * fp;
	FILE * out;
	int c, next;
	
	fp = fopen(argv[1], "rb");
	if(!fp)
	{
		return 1;
	}
	out = fopen(argv[2], "wb");
	if(!out)
	{
		return 1;
	}
	
	while(1)
	{
		c = fgetc(fp);
		if(c == EOF)
		{
			break;
		}
		else if(c == '\n')
		{
			next = fgetc(fp);
			if(next == '\n')
			{
				fputc('\n', out);
				fputc('\n', out);
			}
			else
			{
				fputc(' ', out);
				c = next;
			}
		}
		else
		{
			fputc(c, out);
		}
	}
	fclose(fp);
	fclose(out);
	
	return 0;
}
