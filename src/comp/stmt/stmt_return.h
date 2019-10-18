/*
 * stmt_return.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef COMP_STMT_RETURN_H_
#define COMP_STMT_RETURN_H_

#include "stmt.h"

class ReturnStmt: public Stmt {

public:

	ReturnStmt(){
		this->t = TYPE::Return;
	}


    size_t get_size_bytes() const {
    	return 0;
    }

    void write_to_buf(Buffer& buf) {

    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){

    }


private:
    void toString_sub(std::ostringstream& strm) const {
    	strm << "return";
    }


};




#endif /* COMP_STMT_RETURN_H_ */
