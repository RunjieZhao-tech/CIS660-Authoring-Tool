import point;
import segment;
import triangle;
import java.lang.Math;

class polygon
{
  point[]   vlist;  // List of vertices
  boolean[] matchE; // Which edges are matched
  segment[] dlist;  // List of diagonals
  boolean[] matchD; // Which diagonals were matched
  triangle[] tSet;  // Triangle set for triangulation
  
  point[] SteinerPts; // List of Steiner pts
  segment[] SteinerE; // List of Steiner Edges
 
  public point CurrEP;  // first endpoint of a diagonal to be considered
  public int maxsize;
  public int n = 0;   // number of vertices in this polygon
  public int numDiag = 0;
  public int numTri = 0;
  public int numSteiner = 0;
  public int DTreeHeight = 0;
  public boolean closed = false;
  public boolean triangulated = false;
  public boolean cdiag = false;

  // constructor
  public polygon(int max)
  {
    vlist = new point[max];
    dlist = new segment[max-3];
    tSet = new triangle[max-2];
    matchD = new boolean[max-3];
    matchE = new boolean[max];
    SteinerPts = new point[max/3];
    SteinerE = new segment[2*(max/3)];
    maxsize = max;
  }

  // addVertex(point) : add a vertex to the end of this polygonal chain
  public void addVertex(point p)
  {
    if (n < 100)
    {
      vlist[n] = p;
      n++;
    }
  }

  // addDiag(segment) : add a diagonal to the diagonal set of this polygon
  public void addDiag(segment s)
  {
    dlist[numDiag] = s;
    numDiag++;
    System.out.println(numDiag + " diagonals");
  }

  // Add a triangle to the current triangle set  
  public void addTriangle(triangle t)
  {
    t.center = getCenter(vlist[t.vset[0]],
                         vlist[t.vset[1]],
                         vlist[t.vset[2]]);

    for (int j = 0; j < numTri; j++)
    {
      if (t.adjacent(tSet[j]))
      {
        // Common diagonal index
        int cdi = cdIndex(t,tSet[j]);

        // Update adjacency list of new triangle
        t.AddToAdjList(j,cdi);

        // Update adjacency list of adjacent triangle
        tSet[j].AddToAdjList(numTri,cdi);
      }
    }
    tSet[numTri] = t;
    numTri++;
  }

  public boolean PercDownNotNeeded()
  {
    for (int i = 0; i < numTri; i++)
    {
      triangle t = tSet[i];
      if ((t.Mate == -1) && (t.degree() != 1))
      {
        return false;
      }
    }
    return true;
  }

  public int prev(int Ind)
  {
    for (int i = Ind-1; i >= 0; i--)
    {
      if (!vlist[i].removed) { return i; }
    }
    for (int i = n-1; i > Ind; i--)
    {
      if (!vlist[i].removed) { return i; }
    }
    return -1;
  }

  public int next(int Ind)
  {
    for (int i = 1; i < n; i++)
    {
      if (!vlist[(i+Ind)%n].removed) { return ((i+Ind)%n); }
    }
    return -1;
  }

  /* -- DUAL TREE CONSTRUCTION -- */
  void CreateDualTree() { NodeVisit(0,-1); }

  void NodeVisit(int node, int par)
  {
    if (node == -1) // went beyond a leaf
    {
      if (tSet[par].height > DTreeHeight) { DTreeHeight = tSet[par].height; }
      return;
    }
    if (par == -1)
    { tSet[node].MakeTreeNode(par,-1);
    }
    else { tSet[node].MakeTreeNode(par,tSet[par].height); }

    NodeVisit(tSet[node].LeftChild(),node);
    NodeVisit(tSet[node].RightChild(),node);
  }

  void TriMatch(int t1,int t2)
  {
    tSet[t1].Mate = t2;
    tSet[t2].Mate = t1;
  }

  /* -- TRIANGULATION CONSTRUCTION -- */

  /* return the triangle that an edge belongs to
     assuming that it's the only polygon edge in that
     triangle
  */
  triangle findEdgeTriangle(int vInd1, int vInd2)
  {
    segment edge = new segment(vlist[vInd1], vlist[vInd2]);
    triangle retval = new triangle(-1,-1,-1);

    for (int i = 0;  i < numDiag; i++)
    {
      if (edge.shareEndpt(dlist[i]))
      {
        int udep = getUDEP(vInd1, vInd2, dlist[i]);
        int ueep = getUEEP(vInd1, vInd2, dlist[i]);
        int sep = getSEP(vInd1, vInd2, dlist[i]);
        if (isDiagonal(udep,ueep))
        {
          retval = new triangle(sep,udep,ueep);
          return retval;
        }
      }
    }
    return retval;
  }

  // try to find a free triangle on diagonal d
  triangle findFreeTriangle(segment d)
  {
    triangle retval = new triangle(-1,-1,-1);

    for (int j = 0; j < numDiag; j++)
    {
      if (!d.equals(dlist[j]) && d.shareEndpt(dlist[j]))
      {
        int sep = getSEP(d,dlist[j]);
        int udep1 = getUDEP1(d,dlist[j]);
        int udep2 = getUDEP2(d,dlist[j]);
        
        if (isDiagonal(udep1,udep2))
        {
          retval = new triangle(udep1, udep2, sep);
          return retval;
        }
      }
    }
    return retval;
  }

  point getCenter(point p1, point p2, point p3)
  {
    segment s12 = new segment(p1,p2);
    segment s13 = new segment(p1,p3);
    segment s23 = new segment(p2,p3);

    segment cseg = new segment(s12.midpoint(),s13.midpoint());
    segment c = new segment(cseg.midpoint(),s23.midpoint());
    return(c.midpoint());
  }

  int getUDEP1(segment d1, segment d2)
  {
    point udep;

    if (d1.head().equals(d2.head()) || d1.head().equals(d2.tail()))
    {
      udep = d1.tail();
    }
    else
    {
      udep = d1.head();
    }
    for (int i = 0; i < n; i++)
    {
      if (udep.equals(vlist[i])) { return i; }
    }
    return -1;
  }

  int getUDEP2(segment d1, segment d2)
  {
    point udep;

    if (d2.head().equals(d1.head()) || d2.head().equals(d1.tail()))
    {
      udep = d2.tail();
    }
    else
    {
      udep = d2.head();
    }
    for (int i = 0; i < n; i++)
    {
      if (udep.equals(vlist[i])) { return i; }
    }
    return -1;
  }

  int getSEP(segment d1, segment d2)
  {
    point sep;

    if (d1.head().equals(d2.head()) || d1.head().equals(d2.tail()))
    {
      sep = d1.head();
    }
    else
    {
      sep = d1.tail();
    }
    for (int i = 0; i < n; i++)
    {
      if (sep.equals(vlist[i])) { return i; }
    }
    return -1;
  }

  int getUDEP(int eInd1, int eInd2, segment d)
  {
    point udep;
    segment edge = new segment(vlist[eInd1],vlist[eInd2]);
    if (edge.head().equals(d.head()) || edge.tail().equals(d.head()))
    {
      udep = d.tail();
    }
    else
    {
      udep = d.head();
    }
    for (int i = 0; i < n; i++)
    {
      if (udep.equals(vlist[i])) { return i; }
    }
    return -1;
  }

  int getUEEP(int eInd1, int eInd2, segment d)
  {
    segment edge = new segment(vlist[eInd1],vlist[eInd2]);
    if (edge.head().equals(d.head()) || edge.head().equals(d.tail()))
    {
      return eInd1;
    }
    else
    {
      return eInd2;
    }
  }

  int getSEP(int eInd1, int eInd2, segment d)
  {
    point sep;
    segment edge = new segment(vlist[eInd1],vlist[eInd2]);
    if (edge.head().equals(d.head()) || edge.head().equals(d.tail()))
    {
      return eInd2;
    }
    else
    {
      return eInd1;
    }
  }

  // returns index of common diagonal between t1 & t2
  int cdIndex(triangle t1, triangle t2)
  {
    if (t1.adjacent(t2))
    {
      point ep1 = new point(0,0);
      point ep2 = new point(0,0);
      boolean gotEP1 = false;

      // get the endpoints of the common diagonal
      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          if (t1.vset[i] == t2.vset[j])
          {
            point ep = vlist[t1.vset[i]];
            if (!gotEP1) { ep1 = ep; gotEP1 = true;}
            else { ep2 = ep; break;}
          }
        }
      }

      // get the index of that diagonal
      segment diag = new segment(ep1,ep2);
      for (int i = 0; i < numDiag; i++)
      {
        if (diag.equals(dlist[i]))
        {
          return i;
        }
      }
    }
    return -1;
  }
 
  public boolean isDiagonal(int vInd1, int vInd2)
  {
    segment diag = new segment(vlist[vInd1],vlist[vInd2]);

    for (int i = 0; i < numDiag; i++)
    {
      if (diag.equals(dlist[i]))
      {
        return true;
      }
    }
    return false;
  }

  public boolean isTriangle(triangle t)
  {
    for (int i = 0; i < numTri; i++)
    {
      if (t.equals(tSet[i]))
      {
        return true;
      }
    }
    return false;
  }
}