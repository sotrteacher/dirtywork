typedef struct __attribute__((packed))
{
	unsigned short	LimitL:		16;
	unsigned		BaseL:		24;
	unsigned char	Accessed:	1,
					ReadWrite:	1,
					ConfDir:	1,
					Executable:	1,
					CodeOrData:	1,
					Ring:		2,
					Present:	1,
					LimitH:		4,
					Available:	1,
					LongMode:	1,
					Big:		1,
					Granularity:1,
					BaseH:		8;
} GDTEntry;

static const GDTEntry NullGDT;
static const GDTEntry InitGDT={.CodeOrData=1, .Present=1};

int main(int argc, char const *argv[])
{
	GDTEntry GDT[4];
	GDT[0]=NullGDT;
	for(char i=1;i<4;++i)
		GDT[i]=InitGDT;
	return 0;
}