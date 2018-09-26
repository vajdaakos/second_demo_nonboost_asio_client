cmake_minimum_required(VERSION 3.12)
project(second_demo_nonboost_asio_client)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(certs)
include_directories(jobfiles)

find_package (Threads)

add_executable(second_demo_nonboost_asio_client
        certs/7b4a98ce.0
        Ask_job_message.cpp
        Ask_job_message.h
        Auth_Message_handler.cpp
        Auth_Message_handler.h
        Authentication_Message.cpp
        Authentication_Message.h
        Connection.cpp
        Connection.h
        main.cpp
        Message.cpp
        Message.h
        Message_handler.cpp
        Message_handler.h
        Signer_and_verifier.cpp
        Signer_and_verifier.h
        verbose_verification.cpp
        verbose_verification.h)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        crypto
        ssl
        )