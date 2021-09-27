typedef struct array{
	void * arr;
	int type;
	int size;
	int length;
	void (* addElement)(struct array * a, void * elem);
	void (* printElem)(void * elem);
	void (* print)(struct array * a);
	void * (* get)(struct array * a, int index);
} Array;
struct array * createArray(int type, int size, void printElem(void * elem));

