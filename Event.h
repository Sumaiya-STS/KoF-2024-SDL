class Event
{
private:
    /* data */
    int type;
    int master;
    int time;
public:
    int posX;
    int posY;
    Event(){}
    Event(int _type, int _master, int _time);
    Event(int _type, int _master, int _time, int px, int py);
    ~Event();
    int getType(){return type;}
    int getMaster(){return master;}
    int getTime(){return time;}
};

Event::Event(int _type, int _master, int _time)
{
    type = _type;
    master = _master;
    time = _time;
}
Event::Event(int _type, int _master, int _time, int px, int py)
{
    type = _type;
    master = _master;
    time = _time;
    posX = px;
    posY = py;
}


Event::~Event()
{
}
