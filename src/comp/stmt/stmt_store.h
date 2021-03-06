/*
 * stmt_store.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef STMT_STORE_H_
#define STMT_STORE_H_

#include "stmt.h"

class StoreStmt: public Stmt {

public:
	~StoreStmt(){}

	StoreStmt(std::stringstream& stream){
		this->t = TYPE::Store;

		std::string dst, src, aux;
		stream >> dst >> src >> aux;
		this->src = atoi(src.c_str());
		this->dst = atoi(dst.c_str());
		this->auxiliary = atoi(aux.c_str());
	}

	vertexid_t getSrc() const{
		return src;
	}

	vertexid_t getDst() const{
		return dst;
	}

    vertexid_t getAux() const{
        return auxiliary;
    }



private:

	vertexid_t src;
	vertexid_t dst;
	vertexid_t auxiliary;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "store" << ", " << getDst() << " <- " << getSrc() << ", " << getAux();
    }

};



#endif /* STMT_STORE_H_ */
