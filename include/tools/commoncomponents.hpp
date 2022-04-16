#include "components.hpp"
#include <chrono>
#include <inttypes.h>
namespace acpl{
namespace tools{

class TimerComponent : public StateComponent{
	
public:
	using ComponentWeak_t = ComponentManager::ComponentBase_wptr_t;
	using TimerFunc_t = void(*)(ComponentWeak_t&, TimerComponent&);

	
	TimerComponent(ComponentWeak_t component, TimerFunc_t timerFunction, int64_t timeUntil = 0)
	: StateComponent(Essentiality_t::NON_ESSENTIAL), m_component(component), m_timerFunction(timerFunction), m_timedMiliseconds(timeUntil)
	{}

	int64_t getTimedMilliseconds(){return m_timedMiliseconds;}
	void setTimedMillisecond(int64_t timedMiliseconds){m_timedMiliseconds = timedMiliseconds;}

protected:
	void start() override {	
		using namespace std::chrono;
		m_lastTime = steady_clock::now();
		done();
	}
	void run() override {
		using namespace std::chrono;
		auto now = steady_clock::now();

		if(duration_cast<milliseconds>(now - m_lastTime).count() >= m_timedMiliseconds){
			m_timerFunction(m_component, *this);	
			done();
		}
	}

private:
	TimerFunc_t m_timerFunction;
	std::chrono::time_point<std::chrono::steady_clock> m_lastTime;
	int64_t m_timedMiliseconds;
	ComponentWeak_t m_component;
};

}}