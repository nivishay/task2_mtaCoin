#include "Server.h"

 void* Server::ServerWrapper(void* server_id)
 {
    Server* server = static_cast<Server*>(server_id);
    return server->server_thread(server_id);
 }
Server::Server() {
    pthread_attr_t attr; // Thread attributes
    struct sched_param param; // Scheduling parameters
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = 10; // Set priority to 10 (you can choose any value within the priority range)
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&server_t,&attr,&Server::ServerWrapper, nullptr);//create server thread
}
void Server::addBlock(BLOCK_T block) {
    blockChain.push_back(block);
    ServerBlockMessage(block); 
}
void Server::ServerBlockMessage(BLOCK_T block) {
    std::cout<<"Server: new block added by "<<block.relayed_by<<" attributes: ("<<block.height<<"), Timestamp("<<block.timestamp<<"), hash (0x"<<std::hex<<block.hash<<"), prev_hash(0x"<<std::hex<<block.prev_hash<<"), difficulty ("<<block.difficulty<<"), nonce ("<<block.nonce<<")"<<std::endl;
}
void Server::start(){
    block_to_be_mined = {1, static_cast<int>(time(nullptr)), 0, 0, DIFFICULTY, 0, -1}; // TODO: CHANGE PRIORTY TO BE HIGHT THEN MINERS
    while (true) {
        pthread_cond_broadcast(&newBlockByServer);//notify the miners that a new block is found
        pthread_mutex_lock(&block_hash_found_mutex);
        pthread_cond_wait(&block_hash_found, &block_hash_found_mutex);
        addBlock(block_to_be_mined);
        sleep(2);
        block_to_be_mined = {block_to_be_mined.height + 1,static_cast<int>(time(nullptr)), 0,blockChain.back().hash, DIFFICULTY,1, -1};
        pthread_mutex_unlock(&block_hash_found_mutex);
    }
}