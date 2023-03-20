class point
{
  public int x;
  public int y;

  public boolean removed = false;

  public point(int xc, int yc) {x = xc; y = yc;}
  public point(point p) { x = p.x; y = p.y; }

  public boolean equals(point p)
  {
    return((x == p.x) && (y == p.y));
  }

}