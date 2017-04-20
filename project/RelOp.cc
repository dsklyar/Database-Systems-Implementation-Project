#include <iostream>
#include "RelOp.h"

using namespace std;


ostream& operator<<(ostream& _os, RelationalOp& _op) {
	return _op.print(_os);
}


Scan::Scan(Schema& _schema, DBFile& _file) {
	schema = _schema;
	file = _file;
        file.Open(&file.fileName[0]);
        file.MoveFirst();
}

Scan::~Scan() {
}

ostream& Scan::print(ostream& _os) {
	//return _os << file.GetTableName();
}


Select::Select(Schema& _schema, CNF& _predicate, Record& _constants,
	RelationalOp* _producer) {
	schema = _schema;
	predicate = _predicate;
	constants = _constants;
	producer = _producer;
}

Select::~Select() {
}

bool Select::GetNext(Record& rec) {
    while(producer->GetNext(rec)){
        if(predicate.Run(rec,constants)){ return true; }
    } return false;
}


ostream& Select::print(ostream& _os) {
	_os << "σ [";
	for(int i = 0; i < predicate.numAnds; i++) {
		if(i > 0) {
			_os << " AND ";
		}

		Comparison comp = predicate.andList[i];
		vector<Attribute> atts = schema.GetAtts();
		if(comp.operand1 != Literal) {
			_os << atts[comp.whichAtt1].name;
		} else { // see Record::print for more info
			int pointer = ((int *) constants.GetBits())[comp.whichAtt1 + 1];
			if (atts[comp.whichAtt1].type == Integer) {
				int *myInt = (int *) &(constants.GetBits()[pointer]);
				_os << *myInt;
			} else if (atts[comp.whichAtt1].type == Float) {
				double *myDouble = (double *) &(constants.GetBits()[pointer]);
				_os << *myDouble;
			} else if (atts[comp.whichAtt1].type == String) {
				char *myString = (char *) &(constants.GetBits()[pointer]);
				_os << myString;
			}
		}

		if (comp.op == LessThan) {
			_os << " < ";
		} else if (comp.op == GreaterThan) {
			_os << " > ";
		} else if (comp.op == Equals) {
			_os << " = ";
		} else {
			_os << " ? ";
		}

		if(comp.operand2 != Literal) {
			_os << atts[comp.whichAtt2].name;
		} else { // see Record::print for more info
			int pointer = ((int *) constants.GetBits())[comp.whichAtt2 + 1];
			if (atts[comp.whichAtt1].type == Integer) {
				int *myInt = (int *) &(constants.GetBits()[pointer]);
				_os << *myInt;
			} else if (atts[comp.whichAtt1].type == Float) {
				double *myDouble = (double *) &(constants.GetBits()[pointer]);
				_os << *myDouble;
			} else if (atts[comp.whichAtt1].type == String) {
				char *myString = (char *) &(constants.GetBits()[pointer]);
				_os << "\'" << myString << "\'";
			}
		}
	}
	_os << "] ── " << *producer;
	return _os;
	// return _os << "σ [...] ── " << *producer; // print without predicates
}


Project::Project(Schema& _schemaIn, Schema& _schemaOut, int _numAttsInput,
	int _numAttsOutput, int* _keepMe, RelationalOp* _producer) {
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	numAttsInput = _numAttsInput;
	numAttsOutput = _numAttsOutput;
	keepMe = _keepMe;
	producer = _producer;
}

Project::~Project() {
}
bool Project::GetNext(Record& record) {
    if (producer->GetNext(record)) {
        record.Project(keepMe, numAttsOutput, numAttsInput);		
        return true;
    } return false;
}

ostream& Project::print(ostream& _os) {
	_os << "π [";
	vector<Attribute> atts = schemaOut.GetAtts();
	for(auto it = atts.begin(); it != atts.end(); it++) {
		if(it != atts.begin())
			_os << ", ";
		_os << it->name;
	}
	_os << "]\n\t │\n\t" << *producer;
	return _os;
	// return _os << "π [...]\n\t │\n\t" << *producer; // print without predicates
}


Join::Join(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
	CNF& _predicate, RelationalOp* _left, RelationalOp* _right) {
        
	schemaLeft = _schemaLeft;
	schemaRight = _schemaRight;
	schemaOut = _schemaOut;
	predicate = _predicate;
	left = _left;
	right = _right;
        
        auto copyOut = new Schema();
        copyOut->Append(_schemaOut);
        //old
//        Record temp;
//        if (schemaRight.GetDistincts(schemaRight.atts.at(0).name) <= schemaLeft.GetDistincts(schemaRight.atts.at(0).name)) {
//		leftIsSmaller = false; largerTable = left;
//		while (right->GetNext(temp)) { smallTable.Insert(temp); }
//	} else {
//		leftIsSmaller = true; largerTable = right;
//		while (left->GetNext(temp)) { smallTable.Insert(temp); }
//	}
//	smallTable.MoveToFinish();  
        
        int heapPart_index = 0;
        Record temp;
        if (schemaRight.GetDistincts(schemaRight.atts.at(0).name) <= schemaLeft.GetDistincts(schemaRight.atts.at(0).name)) {
            leftIsSmaller = false; largerTable = left;
            
            int totalRecordMemorySize = 0;
            while(true){
                if(right->GetNext(temp)){
                    totalRecordMemorySize += temp.GetSize();    // increment memory size
                    if(totalRecordMemorySize >= noPages * PAGE_SIZE){ // check if the memory size is over the limit
                        DBFile heapPart; heapPart_index++;
                        auto heap_name = "rightHeap_part_" + to_string(heapPart_index);
                        
                        cout << "creating DBFile "  << heap_name << endl;
                        
                        heapPart.Create(&heap_name[0],Sorted);
                        for(auto it:memoryTable){
                            heapPart.AppendRecord(it);
                        }
                        
                    } else { // not over limit so add it to the TwoWayList
                        
                    }
                }
            }
               
	} else {
            leftIsSmaller = true; largerTable = right;
		
                
	} smallTable.MoveToFinish();
}

Join::~Join() {
}



bool Join::GetNext(Record& _record){
    
    
    
    
    
    
    while(true){
        if(smallTable.AtEnd()){ if(!(largerTable->GetNext(curRecord))){ return false; } smallTable.MoveToStart(); }
        while (!smallTable.AtEnd()){
            if(leftIsSmaller){
                if (predicate.Run(smallTable.Current(), curRecord)){
                    _record.AppendRecords(smallTable.Current(), curRecord, schemaLeft.atts.size(), schemaRight.atts.size());
                    smallTable.Advance(); return true;
                }
            } else {
                if (predicate.Run(curRecord,smallTable.Current())){
                    _record.AppendRecords(curRecord, smallTable.Current(), schemaLeft.atts.size(), schemaRight.atts.size());
                    smallTable.Advance(); return true;
                }
            } smallTable.Advance();
        }
    } return false;
}

ostream& Join::print(ostream& _os) {
	_os << "⋈ [";
	for(int i = 0; i < predicate.numAnds; i++) {
		if(i > 0) {
			_os << " AND ";
		}

		Comparison comp = predicate.andList[i];

		if(comp.operand1 == Left) {
			_os << schemaLeft.GetAtts()[comp.whichAtt1].name;
		} else if(comp.operand1 == Right) {
			_os << schemaRight.GetAtts()[comp.whichAtt1].name;
		}

		if (comp.op == LessThan) {
			_os << " < ";
		} else if (comp.op == GreaterThan) {
			_os << " > ";
		} else if (comp.op == Equals) {
			_os << " = ";
		} else {
			_os << " ? ";
		}

		if(comp.operand2 == Left) {
			_os << schemaLeft.GetAtts()[comp.whichAtt2].name;
		} else if(comp.operand2 == Right) {
			_os << schemaRight.GetAtts()[comp.whichAtt2].name;
		}
	}
	_os << "]";
	_os << ", Number of Tuples = "<<numTuples;
	// _os << "⋈ [...]"; // print without predicates

	_os << "\n";
	for(int i = 0; i < depth+1; i++)
		_os << "\t";
	_os << " ├──── " << *right;

	_os << "\n";
	for(int i = 0; i < depth+1; i++)
		_os << "\t";
	_os << " └──── " << *left;

	return _os;
}


DuplicateRemoval::DuplicateRemoval(Schema& _schema, RelationalOp* _producer) {
	schema = _schema;
	producer = _producer;
}

DuplicateRemoval::~DuplicateRemoval() {
}

bool DuplicateRemoval::GetNext(Record& _record){
    while(producer->GetNext(_record)){
        stringstream key;
        _record.print(key, schema);
        auto it = dupMap.find(key.str());
        if(it == dupMap.end()){
            dupMap[key.str()] = 1;
            return true;
        }
    }
    return false;
}

ostream& DuplicateRemoval::print(ostream& _os) {
	return _os << "δ \n\t │\n\t" << *producer;
}


Sum::Sum(Schema& _schemaIn, Schema& _schemaOut, Function& _compute,
	RelationalOp* _producer) {
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	compute = _compute;
	producer = _producer;
        alreadyCalculatedSum = false;
}

Sum::~Sum() {
}

bool Sum::GetNext(Record& _record){
    if(alreadyCalculatedSum){ return false; }
    int integer_sum = 0;
    double double_sum = 0;
    while(producer->GetNext(_record)){
        int integer_result = 0;
        double double_result = 0;
        
        (compute.Apply(_record,integer_result,double_result) == Integer)?
        integer_sum += integer_result:
        double_sum += double_result;
        
    }
    double sum_result = (double)integer_sum + double_sum; // one of them will be zero
    

    char* recSpace = new char[16];
    int currentPosInRec = sizeof (int) * (2);
    ((int *) recSpace)[1] = currentPosInRec;
    
    if(schemaOut.GetAtts()[0].type == Integer){
            *((int *) (recSpace + currentPosInRec)) = sum_result;
    }else{
            *((double *) (recSpace+ currentPosInRec)) = sum_result;
    }
    
    //*((double *) (recSpace+ currentPosInRec)) = sum_result;
    currentPosInRec += sizeof (double);
    ((int *) recSpace)[0] = currentPosInRec;
    
    Record sumRec;
    
    
    //sumRec.CopyBits( recSpace, currentPosInRec );
    
    //delete [] recSpace;
    
    //cout<<((int*) recSpace)[0]<<endl;
    //cout<<((int*) recSpace)[1]<<endl;
    //cout<<*((double*) (recSpace+8))<<endl;
    
    
    _record.Consume(recSpace);
    alreadyCalculatedSum = true;
    return true;
}

ostream& Sum::print(ostream& _os) {
	_os << "SUM(";
	// do something
	_os << ")\n\t │\n\t" << *producer;;
	return _os;

	// return _os << "SUM(...)\n\t │\n\t" << *producer; // print without predicates
}


GroupBy::GroupBy(Schema& _schemaIn, Schema& _schemaOut, OrderMaker& _groupingAtts,
	Function& _compute,	RelationalOp* _producer) {
	schemaIn = _schemaIn;
	schemaOut = _schemaOut;
	groupingAtts = _groupingAtts;
	groupingAtts.Swap(_groupingAtts);
	compute = _compute;
	producer = _producer;
}

GroupBy::~GroupBy() {
}

bool GroupBy::GetNext(Record& _record){
    if(!mapsCreated){
        int integer_sum = 0;
        double double_sum = 0;
        while(producer->GetNext(_record)){
            stringstream key;
            int integer_result = 0;
            double double_result = 0;

            (compute.Apply(_record,integer_result,double_result) == Integer)?
            integer_sum += integer_result:
            double_sum += double_result;
            double sum_result = (double)integer_sum + double_sum; // one of them will be zero
            
            _record.Project(&groupingAtts.whichAtts[0], groupingAtts.numAtts , schemaOut.GetNumAtts());
            _record.print(key, schemaOut);
            auto it = sumMap.find(key.str());

            if(it != sumMap.end())	{ sumMap[key.str()]+= sum_result; }
            else {
                    sumMap[key.str()] = sum_result;
                    recordMap[key.str()] = _record;
            }

        }
        mapsCreated = true;
    } else {
        if (sumMap.empty()) return false;

        Record temp = recordMap.begin()->second;
        string topr = sumMap.begin()->first;

        char* recSpace = new char[16];
        int currentPosInRec = sizeof (int) * (2);
        ((int *) recSpace)[1] = currentPosInRec;
        *((double *) &(recSpace[currentPosInRec])) = sumMap.begin()->second;
        currentPosInRec += sizeof (double);
        ((int *) recSpace)[0] = currentPosInRec;
        Record sumRec;
        sumRec.CopyBits( recSpace, currentPosInRec );
        delete [] recSpace;

        Record newRec;
        newRec.AppendRecords(sumRec, temp, 1, schemaOut.GetNumAtts()-1);
        recordMap.erase(topr);
        sumMap.erase(topr);
        _record = newRec;
        return true;
    }
}

ostream& GroupBy::print(ostream& _os) {
	_os << "γ [";
	vector<Attribute> atts = schemaOut.GetAtts();
	for(auto it = atts.begin(); it != atts.end(); it++) {
		if(it != atts.begin())
			_os << ", ";

		string attrName = it->name;
		if(attrName == "sum") {
			_os << "SUM(";

			_os << ")";
		} else {
			_os << attrName;
		}
	}
	_os << "]\n\t │\n\t" << *producer;
	return _os;
	// return _os << "γ [...]\n\t │\n\t" << *producer; // print without predicates
}


WriteOut::WriteOut(Schema& _schema, string& _outFile, RelationalOp* _producer) {
	schema = _schema;
	outFile = _outFile;
	producer = _producer;
}

WriteOut::~WriteOut() {
}

bool WriteOut::GetNext(Record& record) {
    bool writeout = producer->GetNext(record);
    if (!writeout) { return false; }
    return writeout;

}

ostream& WriteOut::print(ostream& _os) {
	return _os << endl << "\t" << *producer << endl << endl;
}


ostream& operator<<(ostream& _os, QueryExecutionTree& _op) {
	return _os << "QUERY EXECUTION TREE {" << endl << *_op.root << "}" << endl;
}
