#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>

#include <math.h>
#define BITLENGTH 8

class Node;
using namespace std;
class FingerTable{
public:
	FingerTable(uint8_t nodeId):nodeId_(nodeId)
	{	fingerTable_.resize(BITLENGTH+1);
	}
	void set(size_t index, Node* successor){
		fingerTable_[index]=successor;
	}

	Node* get(size_t index) {
		return fingerTable_[index];
	}	

	std::vector<Node*> fingerTable_;
private:
	uint64_t nodeId_;
};

class Node{
public:
	Node(uint64_t id):id_(id){
		fingerTable_= new FingerTable(id);
	};
	void join(Node *node);
	uint64_t Find(uint64_t key);
	void insertKeyNode(uint64_t key, uint64_t value);  // Inputs key as node id vs value as data id/key
	void insertKeyLocal(uint64_t, uint64_t);
	void remove(uint64_t key);  // remove key from node which stores it
	Node* findNextSuccessorNode(uint64_t id);
	void TransferKeysFromSucc(Node*);
	uint64_t getID()
	{
		return id_;
	}
	void stabilize();
	void stabilize(Node* node);
	bool localLookup(uint64_t key);
	void updateFingerTable();
	void stabilizeAllFingerTables(Node*);
	void displayFingerTable();
	Node* predecessor;
private:
	void insertLocal(uint64_t key, uint64_t value);
	void removeLocal(uint64_t key);  // remove key from node which stores it
private:
	uint64_t id_;
	FingerTable *fingerTable_;
	std::map<uint64_t, uint64_t> localKeys_;
};



void Node::join(Node* node)
{
	//Node will connect now by first looking into finger table of the node we are trying to find

	// This will happen when node passed is the first node in the network

	// First node in the network
	if(node == NULL)
	{
		for(int i=0; i<BITLENGTH; ++i)
		{
			// It's finger entries should store it's own pointer
			fingerTable_->set(i, this);
		}
		predecessor=this; // Node is predecessor of itself only
	}
	else
	{	
		for(int i=0; i<BITLENGTH; ++i)
		{
			// It's finger entries should store it's own pointer
			fingerTable_->set(i, this);
		}
		Node* successor=node->findNextSuccessorNode(id_);

		// fingerTable starts from index=1
		fingerTable_->fingerTable_[0]=successor;


		stabilize(successor);

		// Transfer those keys to newly joined node which are <= it's id
		TransferKeysFromSucc(successor);


		// Update successor's predecessor as this pointer 
		successor->predecessor=this;
		updateFingerTable();
	}

}

void Node::stabilize(Node* successor)
{
		// Update new nodes predeccessor as successor's predecessor
		predecessor=successor->predecessor;

		// Updating predecessor's successor node as newly added node
		successor->predecessor->fingerTable_->fingerTable_[0]=this;

}

void Node::stabilizeAllFingerTables(Node* node)
{
	vector<uint64_t> visitedNodes;
	vector<uint64_t>::iterator it;
	visitedNodes.push_back(fingerTable_->fingerTable_[0]->getID());
	for( int i=1; i <BITLENGTH; ++i)
	{
		//if( find(visitedNodes.begin(), visitedNodes.end(), fingerTable_->fingerTable_[i]->getID()) == visitedNodes.end())
		//{
			visitedNodes.push_back(fingerTable_->fingerTable_[i]->getID());
			node->fingerTable_->fingerTable_[i]->updateFingerTable();

		//}
	}	
}
	
Node* Node::findNextSuccessorNode(uint64_t id)
{
	Node* succ=fingerTable_->fingerTable_[0];
	while(succ->getID() < id_  )
		succ=succ->fingerTable_->fingerTable_[0];

	// Successor node is after completing 0
	//if(succ->fingerTable_->fingerTable_[0]->getID() < id )
	//	return succ->fingerTable_->fingerTable_[0];
	return succ;
}

void Node::updateFingerTable()
{
	for(int i=1; i < BITLENGTH; i++)
	{
		uint64_t val=(id_+pow(2,i));//%pow(2,BITLENGTH);
		int pp=pow(2,BITLENGTH);
		val=val%pp;
			if( val < fingerTable_->fingerTable_[0]->getID() ) 
				fingerTable_->set(i, fingerTable_->fingerTable_[0]);
			// Post 0 circle case
			else if( fingerTable_->fingerTable_[0]->getID() < id_ && val > fingerTable_->fingerTable_[0]->getID() )
				fingerTable_->set(i, fingerTable_->fingerTable_[0]);
			// 0-15 case
			else if( fingerTable_->fingerTable_[0]->getID() < id_ && val < fingerTable_->fingerTable_[0]->getID() && val > 0 )
				fingerTable_->set(i, fingerTable_->fingerTable_[0]);
			else
			{
			
				for( int j=0; j < BITLENGTH; ++j)
				{
					if( val < fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[i]->getID() )
					{	fingerTable_->set(i, fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[i]);
						break;
					}
					// Post 0 circle case
					else if( fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[j]->getID() < id_ && val > fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[j]->getID() )
					{
						fingerTable_->set(i, fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[j]);
						break;
					}
					// 0-15 case
					else if( fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[j]->getID() < id_ && val > fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[j]->getID() && val > 0 )
					{
						fingerTable_->set(i, fingerTable_->fingerTable_[0]->fingerTable_->fingerTable_[j]);
						break;
					}

				}
			}	

	}	

}

void Node::displayFingerTable()
{
	cout<<"-------------------------"<<'\n';
	cout<<"FingerTable       \n";
	cout<<"Index  "<<"Node";
	cout<<"|                       |"<<'\n';
	for(int i=0; i< BITLENGTH; ++i)
	{
		cout<<"|"<<i<<'\t'<<fingerTable_->fingerTable_[i]->getID()<<"|"<<'\n';
	}	
}

// Insert data in a node by inserting the pair of data in it's local structure
void Node::insertKeyLocal(uint64_t key, uint64_t val)
{
	localKeys_.insert(make_pair(key,val));
}

void Node::insertKeyNode(uint64_t key, uint64_t val)
{
	if(key < 0)
	{
		cout<<"Invalid Key. It should be >0";
	}

	// 0th entry is always the successor of node
	Node* successor=fingerTable_->fingerTable_[0];

	// Top of the circle case
	if(predecessor->getID() > id_ && key <= id_ )
	{
		insertKeyLocal(key, val);
	}  // Successor after mod 2^m value
	else if( successor->getID() < id_ && key >= id_ )
		successor->insertKeyLocal(key, val);
	else if( predecessor->getID() < key && key <=id_ )
		insertKeyLocal(key, val);
	else
	{
		while(successor->getID() < key )
			successor=successor->fingerTable_->fingerTable_[0];

		// This is the node where key, value will be stored
		successor->insertKeyLocal(key, val);
	}

}

void Node::TransferKeysFromSucc(Node* succ)
{
	map<uint64_t,uint64_t>::iterator it=succ->localKeys_.begin();

	for(it=succ->localKeys_.begin(); it!= succ->localKeys_.end(); it++)
	{
		if(it->first <= id_)
		{	localKeys_.insert(make_pair(it->first, it->second));
			cout<<"Key and Value which are being migrated to newly joined Node:"<<id_;
			cout<<"Key:"<<it->first<<" Value:"<<it->second<<'\n';
			succ->removeLocal(it->first);
		}
	}

}
// Delete a particular key from a node
void Node::removeLocal(uint64_t key)
{
	std::map<uint64_t,uint64_t>::iterator it = localKeys_.find(key);
	localKeys_.erase(it);

}

// Query a key from a particular node
uint64_t Node::Find(uint64_t key)
{

	//fingerTable_.fingerTable_
	Node* succ=fingerTable_->fingerTable_[0];
	while(key > succ->getID() )
	{		
		cout<<"Nodes visited to find the key"<<succ->getID()<<'\n';
		succ=succ->fingerTable_->fingerTable_[0];
	}

	if(succ->localLookup(key))
		cout<<"Key:"<<key<<"is located at"<<succ->getID();
	// Write recursive lookup algorithm here

}

bool Node::localLookup(uint64_t key)
{

	cout<<"Node"<<id_<<"is being searched";
	map<uint64_t, uint64_t>::iterator it=localKeys_.begin();

	it=localKeys_.find(key);
	
	if(it != localKeys_.end())
		return true;
	else
		return false;

}
#endif
