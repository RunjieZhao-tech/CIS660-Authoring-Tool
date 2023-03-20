import point;
import segment;

class triangle
{
  public int[] adjtri;
  public int[] adjdiag;
  public int[] vset;
  public int numAdj;
  public point center = new point(-1,-1);
  
  public boolean pruned = false;

  public int height = 0;
  public int LC   = -1;
  public int RC   = -1;
  public int Par  = -1;
  public int Mate = -1;

  public triangle(int pi1, int pi2, int pi3)
  {
    vset = new int[3];
    vset[0] = pi1; vset[1] = pi2; vset[2] = pi3;

    adjtri = new int[3];
    adjtri[0] = -1; adjtri[1] = -1; adjtri[2] = -1;

    adjdiag = new int[3];
    adjdiag[0] = -1; adjdiag[1] = -1; adjdiag[2] = -1;

    numAdj = 0;
  }

  public void AddToAdjList(int triIndex, int diagIndex)
  {
    adjtri[numAdj] = triIndex;
    adjdiag[numAdj] = diagIndex;
    numAdj++;
  }

  public boolean adjacent(triangle t)
  {
    int commVert = 0;
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (vset[i] == t.vset[j]) { commVert++; break;}
      }
    }
    if (commVert == 2) { return true; }
    return false;
  }

  public boolean equals(triangle t)
  {
    int commVert = 0;
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (vset[i] == t.vset[j]) { commVert++; break;}
      }
    }
    if (commVert == 3) { return true; }
    return false;
  }

  public void MakeTreeNode(int par, int parheight)
  {
    height = parheight + 1;
    if (adjtri[0] != par)
    {
      LC = 0;
      if (adjtri[1] != par)
      { RC = 1;
      }
      else
      { Par = 1; RC = 2;
      }
    }
    else
    { Par = 0; LC = 1; RC = 2;
    }
  }

  public void prune() { pruned = true; }

  public int LeftDiagonal()
  { if (LC != -1)
    {return adjdiag[LC];
    }
    else { return -1; }
  }

  public int RightDiagonal()
  {
    if (RC != -1)
    {return adjdiag[RC];
    }
    else { return -1; }
  }

  public int Parent()     
  { if (Par != -1)
    {return adjtri[Par];
    }
    else { return -1; }
  }

  public int LeftChild()
  { if (LC != -1)
    {return adjtri[LC];
    }
    else { return -1; }
  }

  public int RightChild()
  { if (RC != -1)
    {return adjtri[RC];
    }
    else { return -1; }
  }

  public int degree() { return numAdj; }
}