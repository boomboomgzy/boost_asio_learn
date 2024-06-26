#include"iocontextpool.h"



Iocontextpool::Iocontextpool(std::size_t size):_iocs(size),_ioc_guards(size),_nextioc_id(0){
    
    std::cout<<"Iocontextpool constructed !"<<std::endl;
    for(std::size_t i=0;i<size;i++){
        _ioc_guards[i]=std::make_unique<work_guard>(boost::asio::make_work_guard(_iocs[i]));
    }
    for(std::size_t i=0;i<size;i++){
        _threads.emplace_back([this,i](){
            _iocs[i].run();
        });
    }
}

Iocontextpool::~Iocontextpool(){
    std::cout<<"Iocontextpool destructed !"<<std::endl;
}

iocontext& Iocontextpool::Getioc(){
    auto& ioc=_iocs.at(_nextioc_id++);
    if(_nextioc_id==_iocs.size()){
        _nextioc_id=0;
    }
    return ioc;
}

void Iocontextpool::stop(){
    for(auto& p:_ioc_guards){
        p->reset();//销毁ioc_guard,从而ioc在处理完回调队列后返回
    }

    for(auto& t:_threads){
        t.join();
    }
}