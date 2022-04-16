#pragma once
#include <list>
#include <memory>
#include <algorithm>

namespace acpl{
namespace tools{

class StateComponent {
	public:
	enum Result_t : int8_t{AWATING, DONE, FAIL};
	enum Essentiality_t : int8_t{NON_ESSENTIAL, ESSENTIAL};
	constexpr static uint16_t NoLimit       = static_cast<uint16_t>(-1);
	constexpr static uint16_t DefaultLimit  = 1000;
	constexpr static uint16_t StrictLimit   = 0;

	constexpr StateComponent(const StateComponent&)                 = delete;
	constexpr StateComponent& operator = (const StateComponent&)    = delete;
	/*static void EnableDebug(bool enable){
		ThisDebugging() = enable;
	}*/

	StateComponent(StateComponent&& other)                          = default;
	StateComponent()                                                = default;
	StateComponent(uint16_t stuckLimit) : m_stuckLimit(stuckLimit), m_stuckCountdown(stuckLimit){}
	StateComponent(Essentiality_t essentiality) : m_essential(essentiality){}
	StateComponent(Essentiality_t essentiality, uint16_t stuckLimit) : m_stuckLimit(stuckLimit), m_stuckCountdown(stuckLimit), m_essential(essentiality){}

	/**
	 * Checks if the intarnal state is still in active progress.
	 * 
	 * @return true, when internal state is still in progress, otherwise false.
	 * @note This function return the same boolian value as operator(), but doesn't execute any active state.
	 **/
	bool isActive() const{
		return (m_state != INVALID_STATE);
	}
	bool isStarting() const{
		return (m_state == START);
	}
	bool isRunning() const{
		return (m_state == RUNNING);
	}
	bool isStopping() const{
		return (m_state == STOP);
	}
	bool isRestarting() const{
		return (m_state == RESTART);
	}

	/**
	 * Restart will force change state to stop which instead of making it inactive will go back to start.
	 * 
	 **/
	void restart(){
		if(isActive() && !isRestarting()){
			reset_stuckCountdown();
			reset_result();
			m_previousResult = DONE;
			m_state = RESTART;
		}
	}

	/**
	 * Restart the internal state to initial value, but only when component is inactive.
	 * 
	 **/
	void reset(){
		if(!isActive()){
			reset_stuckCountdown();
			reset_result();
			m_state = START;
		}
	}

	/**
	 * Go straigt to stop state.
	 * @note Do not use unless it's a last resort.
	 *       (States should never be changed outside of intended place)
	 **/
	void forcestop(){
		if(isActive() && m_state != STOP){
			reset_stuckCountdown();
			reset_result();
			m_previousResult = DONE;
			m_state = STOP;
		}
	}

	Result_t getLastResult() const{
		return m_previousResult;
	}

	void setEssentiality(Essentiality_t essentiality){
		m_essential = essentiality;
	}

	Essentiality_t getEsetiality() const{
		return m_essential;
	}

	
	/**
	 * Run and switches between internal states to execute
	 * start()-->run()-->stop() functions provided for this interface.
	 * 
	 * @return true, while state component is running/active and it's performing internal state switching, otherwise false.
	 **/
	bool operator()(){
		switch(m_state){
			case START:
				m_start();
			break;

			case RUNNING:
				m_run();
			break;

			case STOP:
			case RESTART:
				m_stop();
			break;

			case INVALID_STATE:
				return false;
		}
		return true;
	}

	StateComponent& self(){
		return *this;
	}


	virtual void onStateFinish(){}

	

	virtual ~StateComponent(){}

	protected:
	/**
	 * Prepare intarnal state to be swiched to folowing state.
	 * @example
	 * when called from start(), it will  switch to run().
	 * when called from run(), it will switch to stop(),
	 * when called from stop(), it will make operator() return false;
	 * 
	 * @note After either done() or fail() was called all other calls to this function will be ignored 
	 * @see fail()
	 **/
	void done(){
		if(m_currentResult == AWATING){
			m_currentResult = DONE;
		}
	}

	/**
	 * Prepare intarnal state to be swiched to stop state.
	 * @example
	 * when called from start(), it will switch to stop().
	 * when called from run(), it will switch to stop(),
	 * when called from stop(), it will make operator() return false;
	 * 
	 * @note After either done() or fail() was called all other calls to this function will be ignored 
	 * @see done()
	 **/
	void fail(){
		if(m_currentResult == AWATING){
			m_currentResult = FAIL;
		}
	}
	
	virtual void start(){done();}
	virtual void run(){done();}
	virtual void stop(){done();}

	private:
	void m_start(){
		start();
		switch(m_currentResult){
			case AWATING:
				applyStuckCountdown();
			break;
				
			case DONE: 
				m_previousResult = m_currentResult;
				onStateFinish();
				reset_stuckCountdown();
				reset_result();
				m_state  = RUNNING;
			break; 

			case FAIL: 
				m_previousResult = m_currentResult;
				onStateFinish();
				reset_stuckCountdown();
				reset_result();
				m_state = STOP;
			break; 
		}
	}

	void m_run(){
		run();
		
		switch(m_currentResult){
			case AWATING:break;

			case DONE: 
			case FAIL: 
				m_previousResult = m_currentResult;
				onStateFinish();
				reset_stuckCountdown();
				reset_result();
				m_state  = STOP;
			break; 
		}
	}

	void m_stop(){
		stop();
		m_previousResult = m_currentResult;
		switch(m_currentResult){
			case AWATING:
				applyStuckCountdown();
			break;

			case DONE:
			case FAIL:
				m_previousResult = m_currentResult;
				onStateFinish();
				if(m_state == RESTART){
					reset_stuckCountdown();
					reset_result();
					m_state = START;
				}
				else{
					m_state = INVALID_STATE;
				}
				
			break; 
		}
	}

	void applyStuckCountdown(){
		if(m_stuckCountdown != NoLimit){
			if(m_stuckCountdown > 0){
				--m_stuckCountdown;
			}
			else{
				forcestop();
				m_previousResult = FAIL;
			}
		}
	}

	void reset_stuckCountdown(){
		m_stuckCountdown = m_stuckLimit;
	}

	void reset_result(){
		m_currentResult = AWATING;
	}

	enum State_t : int8_t{START, RUNNING, STOP, RESTART, INVALID_STATE};
	uint16_t m_stuckLimit       = DefaultLimit;
	uint16_t m_stuckCountdown   = m_stuckLimit;
	State_t m_state             = START;
	Result_t m_currentResult    = AWATING;
	Result_t m_previousResult   = AWATING;
	Essentiality_t m_essential  = NON_ESSENTIAL;
/*
	bool thisDebuging(){
		return  (isDebugging() || ThisDebugging());
	}*/

	/*static std::atomic_bool& ThisDebugging(){
		static std::atomic_bool debug = false;
		return debug;
	}*/
};
template <typename Parent>
struct StateComponentParented : public StateComponent{
	public:
	using Parent_t = Parent;
	protected:
	constexpr StateComponentParented(Parent_t& parent) : parent(&parent){}
	constexpr StateComponentParented(Parent_t& parent, uint16_t stuckLimit) : parent(&parent), StateComponent(stuckLimit){}
	constexpr StateComponentParented(Parent_t& parent, Essentiality_t essentiality) : parent(&parent), StateComponent(essentiality){}
	constexpr StateComponentParented(Parent_t& parent, Essentiality_t essentiality, uint16_t stuckLimit) : parent(&parent), StateComponent(essentiality, stuckLimit){}
	Parent_t& getParent(){
		return *parent;
	}
	const Parent_t& getParent() const{
		return *parent;
	}

	private:
	Parent_t* parent;
};

struct ComponentManager{
	using ComponentBase_t = acpl::tools::StateComponent;
	using ComponentBase_wptr_t = std::weak_ptr<ComponentBase_t>;
	using ConstComponentBase_wptr_t = std::weak_ptr<const ComponentBase_t>;
	using ComponentBase_sptr_t = std::shared_ptr<ComponentBase_t>;
	using ConstComponentBase_sptr_t = std::shared_ptr<const ComponentBase_t>;

	template<typename COMPONENT, typename... Ts>
	std::weak_ptr<COMPONENT> addComponent(Ts&&... ts){
		m_components.emplace_back(std::make_shared<COMPONENT>(std::forward<Ts>(ts)...));
		return std::static_pointer_cast<COMPONENT>(m_components.back());
	}
	template<typename COMPONENT>
	void removeComponent(){
		auto it = std::find_if(m_components.begin(), m_components.end(),[](const ComponentBase_sptr_t& component){
			return (dynamic_cast<COMPONENT*>(component.get()) != nullptr);
		});
		if(it != m_components.end()){
			m_components.erase(it);
		}
	}
	void removeComponent(const ComponentBase_wptr_t& component){
		if(!component.expired()){
			auto it = std::find(m_components.begin(), m_components.end(),component.lock());
			if(it != m_components.end()){
				m_components.erase(it);
			}
		}
	}

	template<typename COMPONENT>
	void removeAllComponents(){
		auto it = m_components.begin();
		while(true){
			it = std::find_if(it, m_components.end(),[](const ComponentBase_sptr_t& component){
				return (dynamic_cast<COMPONENT*>(component.get()) != nullptr);
			});
			if(it != m_components.end()){
				it = m_components.erase(it);
			}
			else{
				break;
			}
		}       
		
	}

	

	template<typename COMPONENT>
	std::weak_ptr<COMPONENT> getComponent(){
		auto it = std::find_if(m_components.begin(), m_components.end(),[](const ComponentBase_sptr_t& component){
			return (dynamic_cast<COMPONENT*>(component.get()) != nullptr); //(component->getTypeInfo() == typeid(COMPONENT));
		});
		if(it != m_components.end()){
			return std::static_pointer_cast<COMPONENT>(*it);
		}
		return std::weak_ptr<COMPONENT>();
	}

	
	template<typename COMPONENT>
	std::weak_ptr<const COMPONENT> getComponent() const{
		auto it = std::find_if(m_components.cbegin(), m_components.cend(),[](const ComponentBase_sptr_t& component){
			return (dynamic_cast<COMPONENT*>(component.get()) != nullptr);
		});
		if(it != m_components.end()){
			return std::static_pointer_cast<const COMPONENT>(*it);
		}
		return std::weak_ptr<const COMPONENT>();
	}

		template<typename COMPONENT>
	std::weak_ptr<COMPONENT> getComponent(const COMPONENT& nonWeakComponent){
		auto it = std::find_if(m_components.begin(), m_components.end(),[&nonWeakComponent](const ComponentBase_sptr_t& component){
			return (component.get() == &nonWeakComponent);
		});
		if(it != m_components.end()){
			return std::static_pointer_cast<COMPONENT>(*it);
		}
		return std::weak_ptr<COMPONENT>();
	}

	
	template<typename COMPONENT>
	std::weak_ptr<const COMPONENT> getComponent(const COMPONENT& nonWeakComponent) const{
		
		auto it = std::find_if(m_components.begin(), m_components.end(),[&nonWeakComponent](const ComponentBase_sptr_t& component){
			return (component.get() == &nonWeakComponent);
		});
		if(it != m_components.end()){
			return std::static_pointer_cast<const COMPONENT>(*it);
		}
		return std::weak_ptr<const COMPONENT>();
	}

	template<typename COMPONENT>
	std::vector<ComponentBase_wptr_t>& getAllComponents(std::vector<ComponentBase_wptr_t>& compVec){
		auto it = m_components.begin();
		while(true){
			it = std::find_if(it, m_components.end(),[](const ComponentBase_sptr_t& component){
				return (dynamic_cast<COMPONENT*>(component.get()) != nullptr);
			});
			if(it != m_components.end()){
				compVec.emplace_back(*it);
				++it;
			}
			else{
				break;
			}
		}        
		return compVec;
	}
	template<typename COMPONENT>
	std::vector<ComponentBase_wptr_t> getAllComponents(){
		std::vector<ComponentBase_wptr_t> ret;
		return getAllComponents<COMPONENT>(ret);
	}

	template<typename COMPONENT>
	std::vector<ConstComponentBase_wptr_t>& getAllComponents(std::vector<ConstComponentBase_wptr_t>& compVec) const{
		auto it = m_components.cbegin();
		while(true){
			it = std::find_if(it, m_components.cend(),[](const ComponentBase_sptr_t& component){
				return (dynamic_cast<COMPONENT*>(component.get()) != nullptr);
			});
			if(it != m_components.cend()){
				compVec.emplace_back(*it);
				++it;
			}
			else{
				break;
			}
		}        
		return compVec;
	}

	template<typename COMPONENT>
	const std::vector<ConstComponentBase_wptr_t> getAllComponents() const{
		std::vector<ConstComponentBase_wptr_t> ret;
		return getAllComponents<COMPONENT>(ret);
	}

	template<typename COMPONENT, size_t N>
	std::array<ComponentBase_wptr_t, N> getAllComponents() {
		std::array<ComponentBase_wptr_t, N> ret;
		auto it = m_components.begin();
		for(auto& component : ret){
			it = std::find_if(it, m_components.end(),[](const ComponentBase_sptr_t& component){
				return (dynamic_cast<COMPONENT*>(component.get()) != nullptr);
			});
			if(it != m_components.end()){
				component = *it;
				++it;
			}
			else{
				break;
			}
		}        
		return ret;
	}

	template<typename COMPONENT, size_t N>
	const std::array<ConstComponentBase_wptr_t, N> getAllComponents() const{
		std::array<ConstComponentBase_wptr_t, N> ret;
		auto it = m_components.cbegin();
		for(auto& component : ret){
			it = std::find_if(it, m_components.cend(),[](const ComponentBase_sptr_t& component){
				return (dynamic_cast<COMPONENT*>(component.get()) != nullptr);
			});
			if(it != m_components.cend()){
				component = *it;
				++it;
			}
			else{
				break;
			}
		}        
		return ret;
	}
	std::list<ComponentBase_sptr_t>& getAllComponents(){
		return m_components;
	}
	const std::list<ComponentBase_sptr_t>& getAllComponents() const{
		return m_components;
	}

	bool isEmpty() const{
		return m_components.empty();
	}

	template<typename Callable>
	void visitComponents(const Callable& callable){
		bool bContinue = true;
		for(auto& comp : m_components){
			callable(*comp, bContinue);
			if(!bContinue){
				break;
			}
		}
	}
	template<typename Callable>
	void visitComponents(const Callable& callable) const{
		bool bContinue = true;
		for(const auto& comp : m_components){
			callable(*comp, bContinue);
			if(!bContinue){
				break;
			}
		}
	}

	void setDepencency(ComponentBase_wptr_t dependee, const ConstComponentBase_wptr_t dependency){
		auto dependeeIt = std::find(m_components.begin(), m_components.end(),dependency.lock());
		
		if(dependeeIt != m_components.end()){
			auto dependencyIt = std::find(m_components.begin(), m_components.end(),dependency.lock());
			if(dependencyIt != m_components.end()){
				m_components.splice(++dependencyIt, m_components, dependeeIt);
			}
		}
	}

	void setDepencency(ComponentBase_t& dependee, const ComponentBase_t& dependency){
		auto dependeeIt = std::find_if(m_components.begin(), m_components.end(),[&dependee](const ComponentBase_sptr_t& component){
			return (component.get() == (&dependee));
		});
		
		if(dependeeIt != m_components.end()){
			auto dependencyIt = std::find_if(m_components.begin(), m_components.end(),[&dependency](const ComponentBase_sptr_t& component){
				return (component.get() == (&dependency));
			});
			if(dependencyIt != m_components.end()){
				m_components.splice(++dependencyIt, m_components, dependeeIt);
			}
		}
	}

	virtual ~ComponentManager(){};

	//protected:

	bool handleComponents(){
		for(auto it = m_components.begin(); it != m_components.end();){
			if((*it)->self()() == false){
				if((*it)->getEsetiality() == ComponentBase_t::ESSENTIAL){
					for(auto& component : m_components){
						component->forcestop();
					}
				}
				it = m_components.erase(it);
			}
			else{
				it++;
			}
		}
		return (!m_components.empty());
	}

	private:
	
	std::list<ComponentBase_sptr_t> m_components;
};
	
}}