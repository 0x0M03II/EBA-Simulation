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

#include "Node.h"


EBA::Node::Node() {
    this->nextRID = 0;
    this->state = IDLE;                             // initialize node to IDLE state
    manager = check_and_cast<Manager>(
            getModuleByPath("manager"));

}

EBA::Node::~Node(){}


struct node_states EBA::Node::all_states(void)
{                                                   // return node state
    return node_state {
        this->name,
        this->state,
        this->neighbors,
        this->buffers,
        this->waiting_requests,
        this->message_queue
    };
};


void EBA::Node::handleMessage(message_t *msg)
{
    resolve_message(msg);

    printf("Hello, World!\n");
}

void EBA::Node::resolve_message(message_t* msg)
{
    message_t* message = check_and_cast<message_t*>(msg);
    if (msg->API.request == "BUFREQ") {
        request_buffer(message->bufferName, 1024);
    }
}

void EBA::Node::request_buffer(const std::string& bufferName, int size)
{
    message_t* msg = new message_t();
    msg->sender = name;
    msg->API.request = "BUFREQ";

    // Request a block from manager
    manager->handleBufferRequest(this, size);

    // wait for response
    waiting_requests[bufferName] = msg;
}


void EBA::Node::acknowledge_buffer(buffer_t* buffer)
{
    EV << "Node " << this->name << " received Buffer: " << buffer->owner << "\n";
}


std::string EBA::Node::concat(const std::string s, int r)
{
    return s + std::to_string(r);
}

void EBA::Node::message_template(struct message_template* msg)
{
    msg->sender = this->name;
    msg->RID    = concat(this->name, this->nextRID);
    this->nextRID++;
}


/**
 * @brief Node requests a buffer from Neighbor using manager
 * Let 'space' be an available space in the buffer table for Neighbor
 */
void request_buffer_from(const std::string neighbor, std::string size)
{
    struct message_t* msg = new message_t;
    message_template(&msg);

    msg->space          = space;                                        // undocumented, unsure of purpose
    msg->API.request    = "BUFREQ";
    msg->recipient      = neighbor;                                     // used by manager to lookup buffer

    manager->handleBufferRequest(this, neighbor, msg->bufferName, size);// send buffer message to manager, wait response

    this->waiting_request[msg->bufferName] = msg;                       // now waiting for response, can queue thousands of messages
}


/**
 *
 * @brief resolve the buffer request
 *
 */
void resolve_buffer_request(message_t* msg)
{
    std::string bufferName = concat("BUF_", msg->RID);

    manager->allocateBuffer(bufferName, sizeof(message_t), this->name, msg->sender);

    sendResponse(msg->sender, msg);
}


//void resolve_buffer_request(message_t* msg)
//{
//    std::string buffname = concat("BUF_", msg->RID);
//
//    auto it = this->buffers.find(buffname);
//    if (it != this->buffers.end()) {
//        void* buffer1 = malloc(sizeof(buffer_t));
//
//        buffer_t buf = {
//            .size = -1,
//            .contents = nullptr,
//            .owner = this->name,
//            .intended = msg->sender,
//        };
//
//        this->buffers[buffname] = buff;
//    }
//
//    // sned message
//    sendResponse(msg->sender, msg);
//}


/**
 * @brief Send response to manager - shared memory simulates EBA
 *
 */
void EBA::Node::sendResponse(std::string recipient, message_t* originalMsg)
{
    message_t* respone = new message_t();
    response->sender = this->name;

    response->recipient = recipient;
    response->API.response = "ACK";

    response->bufferName = dynamic_cast<message_t*>(originalMsg)->bufferName;

    manager->handleBufferRequest(this, recipient, response->bufferName);
}



/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                          Node Manager
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

buffer_t* EBA::Manager::getBuffer(const std::string& owner)
{
    for (const auto& entry : ownershipTable) {
        if (entry.second->owner == owner) {
            return entry.second;
        }
    }
    return nullptr;
}


/**
 * @brief self-explanatory
 *
 * nullptr returned from getBuffer()
 * */
void EBA::Manager::sendErrorResponse()
{
    cout << "Failed to find buffer for Neighbor!" << endl;
}


/**
 * @brief Handle communication between nodes
 */
void EBA::Manager::handleBufferRequest(Node* sender, const std::string& recipientName, int size)
{
    if (requestedSize > blockSize) {
        EV << "Size exceed block size " << endl;
        sender->sendErrorResponse();
        return;
    }


    buffer_t* buffer = allocateBlock(sender->getFullName());


    if (block) {
        sender->acknowledgeBuffer(block);
    }
    else {
        sender->sendErrorResponse();
    }
}

void EBA::Manager::releaseBlock(buffer_t* block)
{
    block->owner = "";
    availableBlocks.push_back(block);

    ownershipTable.erase(block->start);

    EV << "Released block " << endl;
}


buffer_t* EBA::Manager::allocateBlock(const std::string& owner)
{
    if (availableBlocks.empty()) {
        EV << "No available memory blocks!\n" << endl;
        return nullptr;
    }

    buffer_t* block = availableBlocks.back();
    availableBlocks.pop_back();

    block->owner = owner;

    ownershipTable[block->start] = block;

    EV << "Allocated block " << endl;
    return block;
}

/**
 * @brief allocate a new buffer the requesting node
 */
void EBA::Manager::allocateBuffer(const std::string& name, int size)
{
    auto it = this->memoryBuffers.find(name);
    if (it != this->memoryBuffers.end()) {
        buffer_t* buf = new buffer_t(size);
        memoryBuffer[name] = buf;
        EV << "Allocated Buiffer" << endl;
    }

//    buffer_t* buf = new buffer_t;
//    buf->size = size;
//    buf->contents = new char[size];
//    buf->owner = owner;
//    buf->intended = intended;
//
//    memoryBuffers[name] = buf;
//    cout << "Buffer Stored!" << endl;

    else {
        EV << "Buffer already exists." << endl
    }
}
