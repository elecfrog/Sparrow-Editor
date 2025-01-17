//
// Created by duhao on 2023/3/27.
//

#ifndef SPARROW_PHYSIC_ERROR_CALL_BACK_H
#define SPARROW_PHYSIC_ERROR_CALL_BACK_H

#endif //SPARROW_PHYSIC_ERROR_CALL_BACK_H
#include <PxPhysicsAPI.h>
#include <iostream>

namespace SPW{
    using namespace physx;
    class PhysicErrorCallback : public PxErrorCallback
    {
    public:
        PhysicErrorCallback(){}
        ~PhysicErrorCallback(){}

        void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override{
            const char* errorCode = NULL;

            switch (code)
            {
                case PxErrorCode::eNO_ERROR:
                    errorCode = "no error";
                    break;
                case PxErrorCode::eINVALID_PARAMETER:
                    errorCode = "invalid parameter";
                    break;
                case PxErrorCode::eINVALID_OPERATION:
                    errorCode = "invalid operation";
                    break;
                case PxErrorCode::eOUT_OF_MEMORY:
                    errorCode = "out of memory";
                    break;
                case PxErrorCode::eDEBUG_INFO:
                    errorCode = "info";
                    break;
                case PxErrorCode::eDEBUG_WARNING:
                    errorCode = "warning";
                    break;
                case PxErrorCode::ePERF_WARNING:
                    errorCode = "performance warning";
                    break;
                case PxErrorCode::eABORT:
                    errorCode = "abort";
                    break;
                case PxErrorCode::eINTERNAL_ERROR:
                    errorCode = "internal error";
                    break;
                case PxErrorCode::eMASK_ALL:
                    errorCode = "unknown error";
                    break;
            }

            PX_ASSERT(errorCode);
            if(errorCode)
            {
                char buffer[1024];
                sprintf(buffer, "%s (%d) : %s : %s\n", file, line, errorCode, message);
                std::cout<<buffer<<std::endl;
            }
        }
    };

}