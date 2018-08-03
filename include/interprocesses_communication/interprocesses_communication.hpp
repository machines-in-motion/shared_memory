#include <set>
#include <string>
#include <map>
#include <memory>
#include <boost/interprocess/managed_shared_memory.hpp>

#define IC_DEFAULT_SIZE 1024


namespace interprocesses_communication {

  namespace admin {


    template<typename T>
    class Segment {

    public:

      Segment(std::string segment_id)
	: segment_id_(segment_id) {

	// note : open_or_create is also supported !
	this->segment_ = new boost::interprocess::managed_shared_memory(boost::interprocess::create_only,
									segment_id.c_str(),
									IC_DEFAULT_SIZE);
	
      }
      
      ~Segment(){
	
	if(this->segment_!=NULL){
	  boost::interprocess::shared_memory_object::remove(this->segment_);
	  delete this->segment_;
	  this->segment_ = NULL;
	}

      }

      void create(const std::string &object_id, const T& initializer) {

	// note: find_or_construct also supported !
	T *instance = this->segment_->construct(object_id.c_str())(initializer);
	this->objects_[object_id] = instance;
	
      }


    private:

      std::string segment_id_;
      std::map<std::string,T*> objects_;
      boost::interprocess::managed_shared_memory *segment_;

    public:

      static void create(std::string segment_id, std::string object_id, T& initializer){

	std::shared_ptr< Segment<T> > segment;
	  
	if (Segment<T>::instances.count(segment_id)>0) {
	  segment = Segment<T>::instances[segment_id];
	}

	else {
	  segment.reset(new Segment<T>(segment_id));
	}
       
	segment->create(object_id,initializer);

      }

    private:

      static std::map< std::string, std::shared_ptr< Segment<T> > > instances;
      
    };


  }


  

  template<typename T>
  bool get(const std::string &segment_id,
	   const std::string &object_id,
	   T &get) {

    boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only,
						       segment_id);
    get = segment.find<T>(object_id);
    
  }

  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   T &set){}
  

}
