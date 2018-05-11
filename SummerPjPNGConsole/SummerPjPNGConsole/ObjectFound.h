#pragma once
/*-------CLASS OBJECTFOUND DEFINITION-------*/
class ObjectFound
{
public:
	ObjectFound(int fRow, int lRow, int wRow, int sCh, int tCh, int cCh);
	~ObjectFound();

	void setFirstRow(int fRw);
	void setLastRow(int lRw);
	void changeChance(int sqC, int trCh, int cirCh);
	void analyze();
	void setHighestColumn(int h);
	void resetChances();
	void setFirstRowSize(int rS);
	int getFirstRowSize();
	int getFirstRow();
	void setLB(int l);
	void setRB(int r);
	int getLB();
	int getRB();
	int depth;
	bool nextRowEmpty;
	void setLastCol(int c);
	int LMSRow;
	int RMSRow;
	int widestNumRow;
	int widestRow;

private:
	int squareChance;
	int triangleChance;
	int circleChance;
	int firstRow;
	int lastRow;
	int LB;	
	int RB;
	int highestColumn;
	int firstRowSize;
	int lastCol;


};
