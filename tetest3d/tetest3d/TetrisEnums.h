
// Movement directions for tetris shapes.

enum D3DTetrisMovementDirection {	LEFT,
									RIGHT,
									ROTATE,
									DOWN,
									DROP, 
									DROPDOWN,
									PAUSEKEY};

// Block Statuses

enum D3DTetrisBlockStatus {	DEAD,
							ALIVE,
							EMPTY};

// Block Colours

enum D3DTetrisBlockColour {	PINK, 
							YELLOW,	
							RED, 
							GREEN, 
							CYAN, 
							BLUE, 
							ORANGE,
							EMPTYBLOCK};