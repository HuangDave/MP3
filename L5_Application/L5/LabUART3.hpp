/*
 * LabUART3.hpp
 *
 *  Created on: Mar 16, 2018
 *      Author: huang
 */

#ifndef LABUART3_HPP_
#define LABUART3_HPP_

#include "LabUART.hpp"

#define UART3 LabUART3::sharedInstance()

class LabUART3: public LabUART {

private:

    static LabUART3 *instance;

public:
    LabUART3();
    virtual ~LabUART3();

    static LabUART3& sharedInstance();
};

#endif /* LABUART3_HPP_ */
