import java.awt.*;
import polyCanvas;

public class T2Q extends java.applet.Applet
{
  private polyCanvas pC;
  boolean algRunning = false;
  Button reset;
  TextField tf;
  Button ctn;

  // Percolate UP-DOWN variables
  Button pud;
  boolean pudDone = false;
  boolean drewSteiner  = false;
  boolean drewDelDiag  = false;
  boolean drewDelEdge  = false;
  boolean pudRunning   = false;
  boolean pudDrawnDual = false;
  boolean pudPercUP    = false;
  boolean pudPercDOWN  = false;

  // Brute-Force variables
  Button bf;
  boolean bfDone = false;
  boolean bfRunning       = false;
  boolean bfSteinerAdded  = false;
  boolean bfSteinerJoined = false;
  
  public boolean action(Event evt, Object obj)
  {
    if (evt.target.equals(reset))
      {
        pC.reset();
        ctn.show();
        bf.hide();
        pud.hide();
        algRunning = false;
        bfRunning = false;
        bfDone = false;
        bfSteinerAdded = false;
        bfSteinerJoined = false;
        bf.setLabel("Brute Force");
        pud.setLabel("Perc UP-DOWN");
        tf.setText("Input and triangulate a simple polygon");
        return true;
      }
    if (evt.target.equals(ctn))
      {
        if (pC.P.closed)
        {
          if (!pC.P.triangulated)
          {
            pC.triangulatePoly();
            pC.DrawDiagonals(Color.red);
            pC.DrawPolygon(Color.blue);
          }
          ctn.hide();
          bf.show();
          pud.show();
          tf.setText("Select algorithm");
        }
      }

    if (evt.target.equals(pud))
      {
        if (!algRunning || pudRunning)
        {
          if (pudDone == true)
          {
            bf.show();
            pC.RemoveQuad();
            pud.setLabel("Perc UP-DOWN");
            tf.setText("Select algorithm");
            algRunning = false;
            pudRunning = false;
            drewSteiner = false;
            drewDelDiag = false;
            drewDelEdge = false;
            pudDone = false;
            pudDrawnDual = false;
            pudPercUP = false;
            pudPercDOWN = false;
          }
          else if (drewDelEdge == true)
          {
            pC.PaintAll("PercUP-DOWN",Color.black);
            pud.setLabel("UnQuad");
            System.out.println(pC.P.numSteiner + " Steiner points");
            tf.setText("Quadrangulated: " + pC.P.numSteiner + " Steiner Points");
            pudDone = true;
          }
          else if (drewSteiner == true)
          {
            tf.setText("Delete matched edges");
            pC.DrawDeleteEdge();
            drewDelEdge = true;
          }
          else if (drewDelDiag == true)
          {
            tf.setText("Insert & Join Steiner Points");
            pC.DrawSteiner(Color.green);
            pC.DrawPolygon(Color.blue);
            drewSteiner = true;
          }
          else if (pudPercDOWN == true)
          {
            tf.setText("Delete Matched Diagonals");
            pC.MarkSegments();
            pC.DrawDeleteDiag();
            drewDelDiag = true;
            if (pC.P.numSteiner == 0) { drewDelEdge = true; }
          }
          else if (pudPercUP == true)
          {
            tf.setText("Altered Matching (Perc. Down)");
            pC.PercolateDOWN();
            pC.DrawDualTree(Color.lightGray);
            pC.DrawMatching(Color.black);
            pudPercDOWN = true;
          }
          else if (pudDrawnDual == true)
          {
            tf.setText("Maximum Matching (Perc. Up)");
            pC.PercolateUP();
            pC.DrawMatching(Color.black);
            pudPercUP = true;
            pudPercDOWN = pC.P.PercDownNotNeeded();
          }
          else if (pC.P.triangulated == true)
          {
            bf.hide();
            algRunning = true;
            pudRunning = true;
            tf.setText("Triangulation dual tree");
            pC.P.CreateDualTree();
            pC.DrawDualTree(Color.lightGray);
            pudDrawnDual = true;
            pud.setLabel("Next");
          }
        }
      }

    if (evt.target.equals(bf))
      {
        if (!algRunning || bfRunning)
        {
          if (bfDone == true)
          {
            pud.show();
            pC.RemoveQuad();
            bf.setLabel("Brute Force");
            tf.setText("Select algorithm");
            algRunning = false;
            bfRunning = false;
            bfDone = false;
            bfSteinerAdded = false;
            bfSteinerJoined = false;
          }
          else if (bfSteinerJoined == true)
          {
            pC.PaintAll("Brute Force",Color.black);
            bf.setLabel("UnQuad");
            System.out.println(pC.P.numSteiner + " Steiner points");
            tf.setText("Quadrangulated: " + pC.P.numSteiner + " Steiner Points");
            bfDone = true;
          }
          else if (bfSteinerAdded == true)
          {
            System.out.println("Joining Steiner pts");
            tf.setText("Join Steiner points");
            pC.JoinBfSteiner(Color.green);
            bfSteinerJoined = true;
          }
          else if (pC.P.triangulated == true)
          {
            pud.hide();
            algRunning = true;
            bfRunning = true;
            System.out.println("Inserting Steiner pts");
            tf.setText("Insert Steiner points");
            pC.DrawBfSteiner(Color.green,true);
            bfSteinerAdded = true;
            bf.setLabel("Next");
          }
        }
        return true;
      }
    return false;
  }

  public void init()
  {
    setLayout(new BorderLayout());
    pC = new polyCanvas();
    pC.resize(350,350);
    pC.setBackground(Color.white);
    add("Center",pC);
    
    Panel p1 = new Panel();
    p1.setLayout(new FlowLayout());
    p1.setBackground(Color.gray);
    ctn = new Button("Continue");
    p1.add(ctn);
    bf = new Button("Brute Force");
    p1.add(bf);
    pud = new Button("Perc UP-DOWN");
    p1.add(pud);
    add("North",p1);

    Panel p2 = new Panel();
    p2.setLayout(new FlowLayout());
    p2.setBackground(Color.gray);
    tf = new TextField("Input and triangulate a simple polygon",30);
    tf.setEditable(false);
    tf.setBackground(Color.yellow);
    p2.add(tf);
    reset = new Button("Reset");
    p2.add(reset);
    add("South",p2);
  }
}
