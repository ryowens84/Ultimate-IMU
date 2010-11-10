#ifndef memory_h
#define memory_h


char memoryBegin(void);
char memoryExists(char * file_name);
int memoryDelete(char * file_name);

class cMemory
{
	public:	
		char file_name[32];

		cMemory(void);
		int create(const char * name, const char * extension);
		int save(char * data);
		void close(void);
		void open(void);
		
	//private:
		struct fat16_file_struct * handle;
		
};

#endif
