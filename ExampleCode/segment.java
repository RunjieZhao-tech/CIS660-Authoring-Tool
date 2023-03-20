class segment
{
  point st,fn;

  // basic constructor
  public segment(point p1,point p2)
  {
    st = p1;
    fn = p2;
  }

  public point midpoint()
  { 
    return(new point( (st.x + fn.x)/2 , (st.y + fn.y)/2 ));
  }

  public boolean equals(segment s)
  {
    return( (st.equals(s.head()) && fn.equals(s.tail())) ||
            (st.equals(s.tail()) && fn.equals(s.head())) );
  }

  public boolean shareEndpt(segment s)
  {
    return( st.equals(s.head()) || fn.equals(s.tail()) ||
            st.equals(s.tail()) || fn.equals(s.head()) );
  }

  public double slope()
  {
    if ((fn.x - st.x) != 0)
    {
      return((fn.y - st.y)/(fn.x - st.x));
    }
    else
    {
      return(1.0e5);
    }
  }

  public point tail() { return st; }
  public point head() { return fn; }
}
  