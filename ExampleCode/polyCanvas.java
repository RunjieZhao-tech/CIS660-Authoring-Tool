import java.awt.*;
import java.lang.Math;
import point;
import segment;
import polygon;
import triangle;

class polyCanvas extends Canvas
{
  polygon P = new polygon(100);
  

  /*-*****************************
   *
   *    GRAPHICS FUNCTIONS
   *    ==================
   */

  public void drawPoint(point p, Color c)
  {
    Graphics g = getGraphics();
    g.setColor(c);
    g.fillOval(p.x - 3, p.y - 3, 6, 6);
    g.setColor(Color.black);
    g.drawOval(p.x - 3, p.y - 3, 6, 6);
  }

  public void drawSegment(segment s, Color c)
  {
    Graphics g = getGraphics();
    g.setColor(c);
    drawPoint(s.tail(),c);
    g.drawLine(s.tail().x,s.tail().y,s.head().x,s.head().y);
    drawPoint(s.head(),c);
  }

  public void reset()
  {
    P = new polygon(100);
    Graphics g = getGraphics();
    g.setColor(Color.white);
    g.fillRect(0,0,350,350);
  }

  void DrawDualTree(Color c)
  {
    for (int i = 0; i < P.numTri; i++)
    {
      triangle t = P.tSet[i];
      if (t.Parent() != -1)
      {
        segment s = new segment(t.center,P.tSet[t.Parent()].center);
        drawSegment(s,c);
      }
      if (t.LeftChild() != -1)
      {
        segment s = new segment(t.center,P.tSet[t.LeftChild()].center);
        drawSegment(s,c);
      }
      if (t.RightChild() != -1)
      {
        segment s = new segment(t.center,P.tSet[t.RightChild()].center);
        drawSegment(s,c);
      }
    }
  }

  void DrawMatching(Color c)
  {
    for (int i = 0; i < P.numTri; i++)
    {
      triangle t = P.tSet[i];
      if (t.Mate != -1)
      {
        segment s = new segment(t.center,P.tSet[t.Mate].center);
        drawSegment(s,c);
      }
    }
  }

  public void PaintAll(String Alg, Color c)
  {
    if (Alg.equals("Brute Force"))
    {
      DrawPolygon(c);
      for (int i = 0; i < P.numDiag; i++)
      {
        drawSegment(P.dlist[i],c);
      }
      DrawBfSteiner(c,false);
      JoinBfSteiner(c);
    }
    if (Alg.equals("PercUP-DOWN"))
    { 
      Graphics g = getGraphics();
      g.setColor(Color.white);
      g.fillRect(0,0,350,350);
      for (int i = 0; i < P.n; i++)
      {
        if (!P.matchE[i])
        {drawSegment(new segment(P.vlist[i],P.vlist[(i+1)%P.n]), c);
        }
      }
      for (int i = 0; i < P.numDiag; i++)
      {
        if (!P.matchD[i]) { drawSegment(P.dlist[i],c); }
      }
      for (int i = 0; i < P.numSteiner; i++)
      {
        drawPoint(P.SteinerPts[i],c);
        drawSegment(P.SteinerE[2*i],c);
        drawSegment(P.SteinerE[(2*i)+1],c);
      }
    }
  }

  public void RemoveQuad()
  {
    Graphics g = getGraphics();
    g.setColor(Color.white);
    g.fillRect(0,0,350,350);
    for (int i = 0; i < P.numDiag; i++)
    {
      drawSegment(P.dlist[i],Color.red);
    }
    for (int i = 0; i < P.n; i++)
    {
      drawSegment(new segment(P.vlist[i],P.vlist[(i+1)%P.n]), Color.blue);
    }
    P.matchD = new boolean[P.maxsize-3];
    P.matchE = new boolean[P.maxsize];
    P.SteinerPts = new point[(P.maxsize/3)];
    P.SteinerE = new segment[2*(P.maxsize/3)];
    P.numSteiner = 0;
    P.DTreeHeight = 0;
  }
 
  /* --- BRUTE FORCE ALGORITHM --- */
  void DrawBfSteiner(Color c, boolean countS)
  {
    for(int i = 0; i < P.numTri; i++)
    {
      segment s1,s2,s3;
      triangle t = P.tSet[i];
      drawPoint(t.center,c);
      s1 = new segment(P.vlist[t.vset[0]], P.vlist[t.vset[1]]);
      s2 = new segment(P.vlist[t.vset[1]], P.vlist[t.vset[2]]);
      s3 = new segment(P.vlist[t.vset[2]], P.vlist[t.vset[0]]);

      drawPoint(s1.midpoint(),c);
      drawPoint(s2.midpoint(),c);
      drawPoint(s3.midpoint(),c);
      
      if (countS == true) { P.numSteiner += 4; }
    }
    if (countS == true) { P.numSteiner -= P.numDiag; }
  }

  void JoinBfSteiner(Color c)
  {
    for(int i = 0; i < P.numTri; i++)
    {
      segment s1,s2,s3;
      triangle t = P.tSet[i];
      s1 = new segment(P.vlist[t.vset[0]], P.vlist[t.vset[1]]);
      s2 = new segment(P.vlist[t.vset[1]], P.vlist[t.vset[2]]);
      s3 = new segment(P.vlist[t.vset[2]], P.vlist[t.vset[0]]);

      drawSegment(new segment(s1.midpoint(),t.center),c);
      drawSegment(new segment(s2.midpoint(),t.center),c);
      drawSegment(new segment(s3.midpoint(),t.center),c);
    }
  }

  /* ----- DELETE EDGE/DIAGONAL DETERMINATION --------- */
  
  public void DrawDeleteEdge()
  {
    Graphics g = getGraphics();
    g.setColor(Color.white);
    g.fillRect(0,0,350,350);
    DrawSteiner(Color.green);
    for (int i = 0; i < P.numDiag; i++)
    {
      if (!P.matchD[i]) 
      {drawSegment(P.dlist[i],Color.red);
      }
    }
    for (int i = 0; i < P.n; i++)
    {
      if (!P.matchE[i])
      {drawSegment(new segment(P.vlist[i],P.vlist[(i+1)%P.n]), Color.blue);
      }
    }
    DrawDualTree(Color.lightGray);
    DrawMatching(Color.black);
  }
  
  public void DrawSteiner(Color c)
  {
    for (int i = 0; i < P.numSteiner; i++)
    {
      drawPoint(P.SteinerPts[i],c);
      drawSegment(P.SteinerE[2*i],c);
      drawSegment(P.SteinerE[(2*i)+1],c);
    }
  }

  public void DrawPolygon(Color c)
  {
    for (int i = 0; i < P.n; i++)
    {
      drawSegment(new segment(P.vlist[i],P.vlist[(i+1)%P.n]), c);
    }
  }

  public void DrawDiagonals(Color c)
  {
    for (int i = 0; i < P.numDiag; i++)
    {
      drawSegment(P.dlist[i],c);
    }
  }

  public void DrawDeleteDiag()
  {
    Graphics g = getGraphics();
    g.setColor(Color.white);
    g.fillRect(0,0,350,350);
    for (int i = 0; i < P.numDiag; i++)
    {
      if (!P.matchD[i]) 
      {drawSegment(P.dlist[i],Color.red);
      }
    }
    for (int i = 0; i < P.n; i++)
    {
      drawSegment(new segment(P.vlist[i],P.vlist[(i+1)%P.n]), Color.blue);
    }
    DrawDualTree(Color.lightGray);
    DrawMatching(Color.black);
  }
  
  public void MarkSegments()
  {
    for (int i = 0; i < P.numTri; i++)
    {
      triangle t = P.tSet[i];
      System.out.println("TRIA: " + i);
      System.out.println("DIAG: " + t.LeftDiagonal());
      System.out.println("LTCH: " + t.LeftChild());
      System.out.println("MATE: " + t.Mate);
      if (t.Mate != -1)
      {
        if (t.LeftChild() != -1)
        {
          if (P.tSet[t.Mate].equals(P.tSet[t.LeftChild()]))
          {
            P.matchD[t.LeftDiagonal()] = true;
          }
        }
        if (t.RightChild() != -1)
        {
          if (P.tSet[t.Mate].equals(P.tSet[t.RightChild()]))
          {
            P.matchD[t.RightDiagonal()] = true;
          }
        }
      }
      else
      {
        for (int j = 0; j < P.n; j++)
        {
          segment s = new segment(P.vlist[j],P.vlist[(j+1)%P.n]);
          segment s0 = new segment(P.vlist[t.vset[0]],P.vlist[t.vset[1]]);
          segment s1 = new segment(P.vlist[t.vset[1]],P.vlist[t.vset[2]]);
          segment s2 = new segment(P.vlist[t.vset[2]],P.vlist[t.vset[0]]);

          if (s.equals(s0) || s.equals(s1) || s.equals(s2))
          {
            findSteiner(j,(j+1)%P.n,t.center);
            P.matchE[j] = true;
            break;
          }
        }
      }
    }
  }

  void findSteiner(int ep1, int ep2, point p)
  {
    segment e = new segment(P.vlist[ep1],P.vlist[ep2]);

    point SteinerP = new point(2*(e.midpoint().x) - p.x,
                               2*(e.midpoint().y) - p.y);

    segment s = new segment(p,SteinerP);
    while ((numIntersect(s)) > 1)
    {
      segment l = new segment(e.midpoint(),SteinerP);
      SteinerP = l.midpoint();
      s = new segment(p,SteinerP);
    }
    P.SteinerPts[P.numSteiner] = SteinerP;
    P.SteinerE[2*P.numSteiner] = new segment(SteinerP,e.head());
    P.SteinerE[(2*P.numSteiner)+1] = new segment(SteinerP,e.tail());
    P.numSteiner++;
  }

  public int numIntersect(segment s)
  {
    int numIntersect = 0;
    for (int i = 0; i < P.n; i++)
    {
      segment e = new segment(P.vlist[i],P.vlist[(i+1)%P.n]);
      if (intersect(s,e)) { numIntersect++; }
    }
    return numIntersect;
  }

  /* --------- PERCOLATE UP-DOWN ALGORITHM ------------ */
  public triangle Parent(triangle t)
  {
    if ((t.Parent() != -1) && (!P.tSet[t.Parent()].pruned))
    {
      return P.tSet[t.Parent()];
    }
    else { return new triangle(-1,-1,-1); }
  }

  public triangle RightChild(triangle t)
  {
    if ((t.RightChild() != -1) &&
        (!P.tSet[t.RightChild()].pruned))
    {
      return P.tSet[t.RightChild()];
    }
    else { return new triangle(-1,-1,-1); }
  }

  public triangle LeftChild(triangle t)
  {
    if ((t.LeftChild() != -1) &&
        (!P.tSet[t.LeftChild()].pruned))
    {
      return P.tSet[t.LeftChild()];
    }
    else { return new triangle(-1,-1,-1); }
  }

  public int degree(triangle t)
  {
    int deg = 0;
    for (int i = 0; i < t.numAdj; i++)
    {
      triangle adjT = P.tSet[t.adjtri[i]];
      if (!adjT.pruned) { deg++; }
    }
    return deg;
  }

  public void PercolateUP()
  {
    int h = 0;
    for (h = P.DTreeHeight; h > 0; h--)
    {
      for (int j = 0; j < P.numTri; j++)
      {
        triangle t = P.tSet[j];
        if ((!t.pruned) && (t.height == h))
        {
          if (degree(Parent(t)) == 1)
          {
            P.TriMatch(j,t.Parent());
            t.prune();
            Parent(t).prune();
          }
          else if (degree(Parent(t)) == 2)
          {
            P.TriMatch(j,t.Parent());
            t.prune();
            Parent(t).prune();
          }
          else if (degree(Parent(t)) == 3)
          {
            if (t.equals(LeftChild(Parent(t))))
            {
              P.TriMatch(j,t.Parent());
              t.prune();
              RightChild(Parent(t)).prune();
              Parent(t).prune();
            }
          }
        }
      }
    }
  }

  public void PercolateDOWN()
  {
    MatchTreeWalk(0);
  }

  public void MatchTreeWalk(int tInd)
  {
    triangle tp;

    if (tInd != -1)
    {
      triangle tcurr = P.tSet[tInd];

      if ((tcurr.Mate == -1) && (tcurr.degree() != 1))
      {
        tcurr.Mate = tcurr.LeftChild();
        tp = P.tSet[P.tSet[tcurr.LeftChild()].Mate];
        P.tSet[tcurr.LeftChild()].Mate = tInd;
        tp.Mate = -1;
      }
      MatchTreeWalk(tcurr.LeftChild());
      MatchTreeWalk(tcurr.RightChild());
    }
  }
        
  /*-************************************************
   *
   *  EVENT HANDLING FUNCTION (calls action functions
   *
   */

  public boolean handleEvent(Event evt)
  {
    if (evt.id == Event.MOUSE_DOWN)
    {
      if (P.closed == false)
      {
        AddToPolygon(new point(evt.x,evt.y));
      }
      else if ((P.cdiag == false) && (P.triangulated == false))
      {
        DiagEndPoint1(new point(evt.x,evt.y));
      }
      else if (P.triangulated == false)
      {
        DiagEndPoint2(new point(evt.x,evt.y));
      }
    }
    return true;
  }

  /*-**********************************************
   *
   *  ACTION FUNCTIONS
   *
   */

  // Create a triangle set from a triangulated polygon
  void CreateTriSet()
  {
    triangle t;
    triangle zt = new triangle(-1,-1,-1);  // degen. tri used as 0

    System.out.println("Creating Triangle Set...");
    for (int i = 0; i < P.n; i++)
    {
      if ((i == 0) && P.isDiagonal(P.n-1,1))
      {
        System.out.println("  Found ear at p0");
        t = new triangle(P.n-1,0,1);
      }

      // edge belongs to a cut ear at vertex P[i+1]
      else if (P.isDiagonal(i,(i+2) % P.n))
      {
        System.out.println("  Found ear at p" + ((i+1)%P.n));
        t = new triangle(i, (i+1)%P.n, (i+2)%P.n);
        i++; // don't need to worry about next edge
      }
      else
      {
        System.out.println("  Found edge triangle at {p" + i + ",p" + ((i+1)%P.n) + "}");
        t = P.findEdgeTriangle(i,(i+1)%P.n);
      }
 
      if (!t.equals(zt) && !P.isTriangle(t))
      { 
        System.out.println("    Adding triangle " + (P.numTri+1));
        P.addTriangle(t);
      }
    }

    // search for free triangles...
    for (int j = 0; j < P.numDiag; j++)
    {
      // ...if all the triangles have not been found
      if (P.numTri < (P.n - 2))
      {
        t = P.findFreeTriangle(P.dlist[j]);
        if (!t.equals(zt) && !P.isTriangle(t))
        {
          System.out.println("  Found free triangle " + t.vset[0] + " " + t.vset[1] + " " + t.vset[2]);
          System.out.println("    Adding triangle " + (P.numTri+1)); 
          P.addTriangle(t);
        }
      }
      else { break; }
    }
    System.out.println("Done: " + P.numTri + " triangles");
  }

  // Get the second endpoint of a possible diagonal
  void DiagEndPoint2(point p)
  {
    for (int i = 0; i < P.n; i++)
    {
      if ( near(P.vlist[i],p) )
      { 
	segment NewDiag = new segment(P.CurrEP,P.vlist[i]);
	boolean goodDiag = true;
	if (!P.CurrEP.equals(P.vlist[i]))
        {
          for (int j = 0; j < P.n; j++)
	  {
            // considered intersection edge
            segment Current = new segment(P.vlist[j],P.vlist[(j+1) % P.n]);

            if (NewDiag.equals(Current)) { goodDiag = false; break; }

	    // if the considered edge doesn't share endpoint with diagonal
	    if ( !Current.shareEndpt(NewDiag) )
	    { 
              if (intersect(NewDiag,Current))
	      { 
	        goodDiag = false;
	        break;
	      }
	      else if (!MinPoly(NewDiag.midpoint()))
	      {
	        goodDiag = false;
	        break;
	      }
            }
	  }

          if (!goodDiag)
          {
            P.cdiag = false; 
            drawPoint(P.CurrEP, Color.blue);
            return;
          }

          for (int j = 0; j < P.numDiag; j++)
          {
            if (NewDiag.equals(P.dlist[j])) { goodDiag = false; break; }
            if (!NewDiag.shareEndpt(P.dlist[j]) &&
                 intersect(NewDiag,P.dlist[j]))
            { 
              goodDiag = false;
              break;
            }
          }
          if (!goodDiag)
          {
            P.cdiag = false;
            drawPoint(P.CurrEP, Color.blue);
            return;
          }

        }
        else { goodDiag = false; }

        if (goodDiag)
        {
          drawSegment(NewDiag,Color.red);
          drawPoint(NewDiag.head(), Color.blue);
          drawPoint(NewDiag.tail(), Color.blue);
          P.cdiag = false;
          P.addDiag(NewDiag);
	  if (P.numDiag == (P.n - 3))
	  {
            P.triangulated = true;
            System.out.println("P triangulated");
            CreateTriSet();
            System.out.println("ADJACENCIES");
            for (int k = 0; k < P.numTri; k++)
            {
              System.out.print("T" + (k+1) + ": ");
              triangle T = P.tSet[k];
              for (int l = 0; l < 3; l++)
              {
                System.out.print(T.vset[l] + " ");
              }
              System.out.print(": ");

              for (int l = 0; l < T.numAdj; l++)
              {
                System.out.print("T" + (T.adjtri[l]+1) + " ");
              }

              // System.out.print(T.numAdj + " adjacent");
              System.out.println("");
            }
          }
        }
        else { P.cdiag = false; drawPoint(P.CurrEP, Color.blue); }
      }
    }
  }

  // Get the first endpoint of a possible diagonal
  void DiagEndPoint1(point p)
  {
    for (int i = 0; i < P.n; i++)
    {
      if ( near(P.vlist[i],p) )
      { 
        P.cdiag = true;
        P.CurrEP = P.vlist[i];
        drawPoint(P.vlist[i],Color.red);
        break;
      }
    }
  }

  // Attempt to add a new point p to the polygon P
  void AddToPolygon(point p)
  {
    int i;
    boolean simple = true;

    if (P.n == 0)  // first point in the polygon
    {
      P.addVertex(p);
      drawPoint(p,Color.blue);
    }
    else
    {
      segment NewEdge;
      boolean close = false;
      if (!near(P.vlist[P.n-1],p))
      {
        if ( near(P.vlist[0],p) || (P.n == P.maxsize) )
        {
          // potential closing edge
          NewEdge = new segment(P.vlist[P.n-1],P.vlist[0]);
	  close = true;  P.closed = true;
          if (NewEdge.equals(new segment(P.vlist[0], P.vlist[1])))
          {
          simple = false;
          }
        }
        else
        {
          // potential next edge
          NewEdge = new segment(P.vlist[P.n-1], p);
        }

        for (i = 0; i < (P.n-2); i++)
        {
	  if (close == true) { i++; close = false; }
	
	  if ( near(P.vlist[i+1],p) ) { simple = false; break; }
          else
          {
            // considered intersection edge
            segment CurrEdge = new segment(P.vlist[i],P.vlist[i+1]);
        
	    if (intersect(NewEdge,CurrEdge)) { simple = false; break;}
          }
        }

        if (simple)
        {
          drawSegment(NewEdge,Color.blue);
          if (NewEdge.head().equals(P.vlist[0]))
          {
            System.out.println("P closed: " + P.n + " vertices");
          }
	  else { P.addVertex(NewEdge.head()); }
        }
        else { P.closed = false; }
      }
    }
  }

  /*-***********************************************************
   *
   *  SUPPORT FUNCTIONS
   *
   */

  // test whether the jth vertex of P is an endpoint of the given segment
  boolean isEndpt(int j, segment s)
  {
    return ( P.vlist[j].equals(s.head()) || P.vlist[j].equals(s.tail()) );
  }

  boolean MinPoly(point p)
  {
    if (P.closed)
    {
      // SECOND: Vertical Lines
      segment jseg = new segment(p, new point(p.x,350));
      int numIntersect = 0;
      for(int i = 0; i < P.n; i++)
      {
        // considered intersection edge
        segment CurrEdge = new segment(P.vlist[i],P.vlist[(i+1) % P.n]);

        if (intersect(CurrEdge,jseg))
        {
          numIntersect++;
        }
      }
      if (MaxVertLineClear(p))
      {
        if ((numIntersect % 2) == 1) { return true; }
      }

      jseg = new segment(p, new point(p.x,0));
      numIntersect = 0;
      for(int i = 0; i < P.n; i++)
      {
        // considered intersection edge
        segment CurrEdge = new segment(P.vlist[i],P.vlist[(i+1) % P.n]);

        if (intersect(CurrEdge,jseg))
        {
          numIntersect++;
        }
      }
      if (MinVertLineClear(p))
      {
        if ((numIntersect % 2) == 1) { return true; }
      }
      
      // SECOND: Horizontal Line
      jseg = new segment(p, new point(350,p.y));
      numIntersect = 0;
      for(int i = 0; i < P.n; i++)
      {
        // considered intersection edge
        segment CurrEdge = new segment(P.vlist[i],P.vlist[(i+1) % P.n]);

        if (intersect(CurrEdge,jseg))
        {
          numIntersect++;
        }
      }
      if (MaxHorzLineClear(p))
      {
        if ((numIntersect % 2) == 1) { return true; }
      }

      jseg = new segment(p, new point(0,p.y));
      numIntersect = 0;
      for(int i = 0; i < P.n; i++)
      {
        // considered intersection edge
        segment CurrEdge = new segment(P.vlist[i],P.vlist[(i+1) % P.n]);

        if (intersect(CurrEdge,jseg))
        {
          numIntersect++;
        }
      }
      if (MinHorzLineClear(p))
      {
        if ((numIntersect % 2) == 1) { return true; }
      }
    }
    return false;
  }

  boolean MaxVertLineClear(point p)
  {
    for (int i = 0; i < P.n; i++)
    {
      if ((P.vlist[i].y > p.y) && (P.vlist[i].x == p.x))
      {
        return false;
      }
    }
    return true;
  }

  boolean MinVertLineClear(point p)
  {
    for (int i = 0; i < P.n; i++)
    {
      if ((P.vlist[i].y < p.y) && (P.vlist[i].x == p.x))
      {
        return false;
      }
    }
    return true;
  }

  boolean MaxHorzLineClear(point p)
  {
    for (int i = 0; i < P.n; i++)
    {
      if ((P.vlist[i].x > p.x) && (P.vlist[i].y == p.y))
      {
        return false;
      }
    }
    return true;
  }

  boolean MinHorzLineClear(point p)
  {
    for (int i = 0; i < P.n; i++)
    {
      if ((P.vlist[i].x < p.x) && (P.vlist[i].y == p.y))
      {
        return false;
      }
    }
    return true;
  }

  // test whether a point is in the polygon
  boolean AinPoly(point p)
  {
    if (P.closed)
    {
      segment[] IntSet = new segment[P.n];

      // FIRST: Try two vertical lines
      segment jseg = new segment(p, new point(p.x,350));
      int numIntersect = 0;
      int setSize = 0;
      for(int i = 0; i < P.n; i++)
      {
        // considered intersection edge
        segment CurrEdge = new segment(P.vlist[i],P.vlist[(i+1) % P.n]);

        if (intersect(CurrEdge,jseg))
        {
          boolean goodInt = true;
          for (int j = 0; j < setSize; j++)
          {
            if (CurrEdge.shareEndpt(IntSet[j]))
            {
              System.out.println("shared endpoints");
              byte trn1=0,trn2=0;
              int sepI = P.getSEP(CurrEdge,IntSet[j]);
              
              trn1 = turn(p,P.vlist[sepI],P.vlist[(sepI+1)%P.n]);
              if (sepI != 0)
              {trn2 = turn(p,P.vlist[0],P.vlist[P.n-1]);
              }
              else
              {trn2 = turn(p,P.vlist[sepI],P.vlist[sepI-1]);
              }
                
              if (P.vlist[sepI].x == p.x)
              {
                System.out.println("bad intersection");
                goodInt = false;
                if (trn1 == trn2) 
                { System.out.println("really bad int");
                  numIntersect--;
                }
              }
            }
          }
          if (goodInt)
          {
            System.out.println("good intersection");
            IntSet[setSize] = CurrEdge;
            setSize++; numIntersect++;
          }
        }
      }
      if ((numIntersect % 2) == 0) { return false; }
    }
    return true;
  }

  // returns true if p1 and p2 are "near" eachother
  boolean near(point p1, point p2)
  {
    return ((Math.abs(p1.x - p2.x) <= 3) &&
	    (Math.abs(p1.y - p2.y) <= 3));
  }

  public void triangulatePoly()
  {
    P.dlist = new segment[P.maxsize-3];
    P.tSet = new triangle[P.maxsize-2];
    P.cdiag = false;
    P.numTri = 0;
    P.numDiag = 0;

    while (P.numDiag <= (P.n-3))
    {
      int ear = findEarInPoly();
      int prevV = P.prev(ear);
      int nextV = P.next(ear);

      if (ear == -1)
      {
        System.out.println("New tria --> " + prevV + " " + ear + " " + nextV);
        System.out.println("New diag --> " + prevV + " " + nextV);
      }
      segment s = new segment(P.vlist[prevV],P.vlist[nextV]);
      System.out.println("In Poly? " + MinPoly(s.midpoint()));
      P.addDiag(s);
      drawSegment(s,Color.red);
      triangle t = new triangle(prevV,ear,nextV);
      P.addTriangle(t);
      P.vlist[ear].removed = true;
    }
    P.triangulated = true;
  }

  private int findEarInPoly()
  {
    for (int i = 0; i < P.n; i++)
    {
      if (!P.vlist[i].removed)
      {
        if (isEar(i)) { return i; }
      }
    }
    return -1;
  }
  
  private boolean isEar(int i)
  {
    boolean EAR = true;
    segment d = new segment(P.vlist[P.prev(i)],P.vlist[P.next(i)]);
    for (int j = 0; j < P.n; j++)
    {
      segment c = new segment(P.vlist[j],P.vlist[(j+1)%P.n]);
      if ((!d.shareEndpt(c)) && intersect(d,c))
      {
        EAR = false; break;
      }
      if (!MinPoly(d.midpoint())) { EAR = false; break; }
    }
    return(EAR);
  }

  /*
   * returns 1 if abc is a left turn
   *        -1 if abc is a right turn
   *         0 if a, b, and c are collinear
   */

  byte turn(point a, point b, point c)
  {	
    point[] T = { a , b , c };  // Triangle defined by points
    int AreaT = 0;  // Area of triangle
    
    AreaT += (T[0].x * T[1].y) - (T[0].y * T[1].x);
    AreaT += (T[1].x * T[2].y) - (T[1].y * T[2].x);
    AreaT += (T[2].x * T[0].y) - (T[2].y * T[0].x);

    if (AreaT > 0) return 1;  // left turn
    if (AreaT < 0) return -1; // right turn
    return 0;		      // collinear
  }

  // returns true if s1 and s2 intersect
  boolean intersect(segment s1, segment s2)
  {
    int t1h,t1t,t2h,t2t;

    if (!bbintersect(s1,s2)) // Quick rejection
    {
      return false;
    }
    else // Bounding boxes of edges intersect
    {
      if (!s1.equals(s2))
      {
        t1h = turn(s1.head(),s1.tail(),s2.head());
        t1t = turn(s1.head(),s1.tail(),s2.tail());
    
        if ((t1h == t1t)&&(t1h != 0)&&(t1t != 0)) { return false; }
      
        t2h = turn(s2.head(),s2.tail(),s1.head());
        t2t = turn(s2.head(),s2.tail(),s1.tail());

        if ((t2h == t2t)&&(t2h != 0)&&(t2t != 0)) { return false; }
      }
    }
    return true;
  }
 
  // Intersection of bounding boxes of two line segments
  boolean bbintersect(segment s1, segment s2)
  {
    int xmin1,xmax1,ymin1,ymax1;
    int xmin2,xmax2,ymin2,ymax2;

    // min and max x-coords of s1
    if (s1.head().x >= s1.tail().x)
      { xmin1 = s1.tail().x; xmax1 = s1.head().x;}
    else
      { xmin1 = s1.head().x; xmax1 = s1.tail().x;}

    // min and max y-coords of s1
    if (s1.head().y >= s1.tail().y)
      { ymin1 = s1.tail().y; ymax1 = s1.head().y;}
    else
      { ymin1 = s1.head().y; ymax1 = s1.tail().y;}

    // min and max x-coords of s2
    if (s2.head().x >= s2.tail().x)
      { xmin2 = s2.tail().x; xmax2 = s2.head().x;}
    else
      { xmin2 = s2.head().x; xmax2 = s2.tail().x;}

    // min and max y-coords of s2
    if (s2.head().y >= s2.tail().y)
      { ymin2 = s2.tail().y; ymax2 = s2.head().y;}
    else
      { ymin2 = s2.head().y; ymax2 = s2.tail().y;}

    return ((xmax1 >= xmin2) && (xmax2 >= xmin1) &&
	    (ymax1 >= ymin2) && (ymax2 >= ymin2));
  }
}
