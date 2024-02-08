#define tuple struct Tuple

struct Tuple
{
    int x;
    int y;
};

enum Ship
{
    CARRIER=5,
    BATTLESHIP=4,
    CRUISER=3,
    SUBMARINE=3,
    DESTROYER=2,
    NONE=0
};

enum State
{
    SUNK = '#',
    HIT = 'X',
    MISS = 'O',
    UNSHOT = '~'
};

struct Cell
{
    enum Ship aShip;
    enum State aState;
};