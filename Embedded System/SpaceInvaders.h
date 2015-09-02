


typedef enum {dead, dying, alive, paused} status;
struct state {
	int x;
	int y;
	int w;
	int h;
	const unsigned short *image;
	status life;
	int dietime;
	int doublehit;
	int play;
	int song;
};
typedef struct state stype;

extern stype characters[];
