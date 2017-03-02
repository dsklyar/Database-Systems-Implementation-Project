#include "QueryCompiler.h"
#include "QueryOptimizer.h"
#include "Schema.h"
#include "ParseTree.h"
#include "Record.h"
#include "DBFile.h"
#include "Comparison.h"
#include "Function.h"
#include "RelOp.h"

using namespace std;


QueryCompiler::QueryCompiler(Catalog& _catalog, QueryOptimizer& _optimizer) :
	catalog(&_catalog), optimizer(&_optimizer) {
	catalog = &_catalog;
	optimizer = &_optimizer;
}

QueryCompiler::~QueryCompiler() {

}

void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
	FuncOperator* _finalFunction, AndList* _predicate,
	NameList* _groupingAtts, int& _distinctAtts,
	QueryExecutionTree& _queryTree) {
	cout<<"COMPILING.."<<endl;

	//preprocessing 1
	
	// create a SCAN operator for each table in the query
        vector<Scan> _scans;
        vector<Select> _selects;
        vector<Join> _joins;
        //saving the head
        TableList* head = new TableList();
        head = _tables;
        DBFile _db;
        
        while(_tables != NULL){
            Schema _schema;
            string _table = _tables->tableName;
            catalog->GetSchema(_table,_schema);
            Scan * _scan = new Scan(_schema,_db);
            _scans.push_back(*_scan);
            
            AndList* AndListCopy = new AndList();
            AndListCopy = _predicate;
            
            Record _record;
            CNF _cnf;


            if(_cnf.ExtractCNF(*AndListCopy,_schema,_record) == 0){
                Select _select(_schema,_cnf,_record,_scan);
                while(AndListCopy != NULL){
                    if(ConditionOnSchema(*AndListCopy, _schema)){
                        ComparisonOp* cOp = AndListCopy->left;

                
                        //NAME op (FLOAT,INTEGER) ex: p.size < 50
                        if(cOp->left->code == NAME && cOp->code != EQUALS && cOp->right->code != NAME)
                        {
                            string _attributeName(cOp->left->value);
                            cout<<"selection: "<<_attributeName<<endl;
                            _schema._noTuples /= 3;
                           

                        }
                        else if(cOp->left->code == NAME && cOp->code == EQUALS && cOp->right->code != NAME)
                        {
                            string _attributeName(cOp->left->value);
                            cout<<"selection=: "<<_attributeName<<endl;
                            unsigned int _noDistinct = 0;
                            catalog->GetNoDistinct(_table,_attributeName,_noDistinct);
                            _schema._noTuples /= _noDistinct;
                        }
                    }
                    AndListCopy = AndListCopy->rightAnd;
                }
            }
            
            
            
            _tables =_tables->next;
        } _tables = head;

	// call the optimizer to compute the join order
	OptimizationTree* root;
        _tables = head;
	optimizer->Optimize(_tables, _predicate, root);
	// create join operators based on the optimal order computed by the optimizer
//       _tables = head;
//       while(_tables->next != NULL && _tables->next != NULL){
//           
////           table = _tables->tableName;
////           string table2 = _tables->next->tableName;
////           Schema schema_one, schema_two, schema_out;
////           CNF _cnf;
////           catalog->GetSchema(table,schema_one);
////           catalog->GetSchema(table2,schema_two);
////           _cnf.ExtractCNF(_predicate,schema_one,schema_two);
////           RelationalOp * _left = new RelationalOp();
////           RelationalOp * _right = new RelationalOp();
////           Join _join(schema_one,schema_two,schema_out,_cnf,_left,_right);
//           //end
//           _tables = _tables->next;
//       }
        
	// create the remaining operators based on the query

	// connect everything in the query execution tree and return

	// free the memory occupied by the parse tree since it is not necessary anymore
}
