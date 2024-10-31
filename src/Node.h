//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef NODE_H_
#define NODE_H_

#include <map>
#include <string>
#include <omnetpp.h>

using namespace omnetpp;

namespace EBA
{

#define BUFFER_SIZE 1024

    struct buffer_t
    {
        int size;
        char* contents;

        buffer_t() : size(size), start(start), owner(owner) {}

        ~buffer_t() {
            delete[] contents;
        }
    };

    /**
     * @brief node state struct to hold active state for
     * a given node
     */
    struct node_state
    {                                       // no user-declared constructors allowed
        std::string name;
        std::string state;
        cQueue message_queue;
        std::map<std::string, void*> buffers;
        std::map<std::string, std::string> neighbors;
        std::map<std::string, cMessage*> waiting_requests;
    };

    /**
     * @brief message template struct to format messages
     */
    struct message_t : public cMessage
    {                                       // no user-declared constructors allowed
        int RID;
        std::string sender;
        std::string recipient;
        std::string space;
        union {
            std::string request;
            std::string response;
        } API;
    };

    /* EBA Node State */
    enum eba_state { IDLE, RESPOND };       // EBA State: IDLE = 0 , RESPOND = 1

    class Manager;

    /**
     * @brief The Node class defines the behavior for an EBA network node
     * nodes should be isolated, requiring the use of simulator facilities
     * to communicate with each other
     */
    class Node: public cSimpleModule {
    private:
        int nextRID;                    // manager is for exchanges to isolate and use shared memory
        eba_state state;
        std::string name;
        Manager* manager;
        std::map<std::string, std::string> neighbors;
        std::map<std::string, cMessage*> waiting_requests;

    protected:

        std::string concat(const std::string, int) const;
        struct node_states all_states(void) const;                          // return active node state
        void resolve_message(cMessage* msg) const;                          // resolve request
        void acknowledge_buffer(cMessage* msg) const;                       //
        void resolve_buffer_request(cMessage* msg) const;                   // allocate buffer, send response msg
        void sendResponse(std::string recipient, message_t* originalMsg);
        void request_buffer_from(const std::string neighbor, std::string space);
        void message_template(struct message_t* msg) const;          // default message template
        void request_buffer(const std::string& bufferName, int size);

        /* virtual functions override behavior of cSimpleModule */
        virtual void handleMessage(cMessage* msg) override;
        virtual void processMessage(cMessage* msg) override;


    public:
        Node();
        virtual ~Node();
    }; // Class -> Node


    class Manager : public cSimpleModule {
    private:
        char* memoryPool;
        int poolSize;
        int blockSize;
        std::map<std::string, buffer_t*> ownershipTable;         // Manager uses shared memory space
        std::vector<buffer_t*> availableBlocks;

    public:
        Manager() : memoryPool(nullptr), poolsize(10240), blocksize(1024) {
            memoryPool = new char[poolSize];
            EV << "Memory Pool of size " << poolsize << " allocated!\n" << endl;

            int numBlocks = poolSize / blockSize;
            for (int i = 0; i < numBlocks; ++i) {

                char* blockStart = memoryPool + (i * blockSize);
                buffer_t* block = new buffer_t(blockSize, blockStart, "");
                availableBlocks.push_back(blcok);
            }
        }

        ~Manager() {

            if (this->memoryPool) {
                delete[] memoryPool;
            }

            for (auto& entry : ownershipTable) {
                delete entry.second;
            }
        }

        void sendErrorResponse();
        void releaseBlock(buffer_t* block);
        buffer_t* allocateBlock(const std::string& owner);
        buffer_t* getBuffer(const std::string& bufferName);
        void handleBufferRequest(Node* sender, const std::string& recipientName, const std::string& bufferName);
        void allocateBuffer(const std::string& name, int size, const std::string& owner, const std::string& intended);

    };

} // Namespace EBA

Define_Module(Node);

#endif /* NODE_H_ */
