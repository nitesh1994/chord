#include "Node.h"

#include <stdint.h>
#include <iostream>

#include <math.h>
// define this properly
//#define val(id, i ) (id + pow(2,i))%BITLENGTH
using namespace std;


/*void FingerTable::displayFingerTable()
{
	int i;
	for(i=1; i<fingerTable_.size(); ++i)
	{
		cout<<"Finger Entry->"<<i<<"Finger Node"<<get(i)<<'\n';
	} 
}*/

int main()
{
	Node* node= new Node(15);

	node->join(NULL);

	Node* node1= new Node(68);
	node1->join(node);
	node1->stabilizeAllFingerTables(node);
	node1->insertKeyNode(42, 88);
	node1->insertKeyNode(48, 88);
	node1->insertKeyNode(67, 88);
	Node* node2= new Node(44);
	node2->join(node);
	//node2->insertKeyNode(42, 88);
	node1->stabilizeAllFingerTables(node2);
	Node* node3= new Node(50);
	node3->join(node1);
	node3->stabilizeAllFingerTables(node);
	node3->stabilizeAllFingerTables(node2);
	//Node* node4= new Node(150);
	//node4->join(node1);
	//node4->stabilizeAllFingerTables(node);
	//node4->stabilizeAllFingerTables(node3);
	cout<<node->getID();
	node->displayFingerTable();
	cout<<node1->getID();
	node1->displayFingerTable();
	cout<<node2->getID();
	node2->displayFingerTable();
	cout<<node3->getID();
	node3->displayFingerTable();

	node->Find(48);
	node->Find(67);
	//cout<<node4->getID();
	//node4->displayFingerTable();
	//node->insertKeyLocal(7,99);
	//cout<<node->fingerTable_->fingerTable_[0]->getID();
	
	return 0;

}
