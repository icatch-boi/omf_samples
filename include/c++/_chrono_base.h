#pragma once
#include "_typedef.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <type_traits>
#include <sys/time.h>

namespace omf{
	namespace chrono {
	/// Clock
	#if __cplusplus >= 201103L
		typedef std::chrono::steady_clock __clock;
	#else
		typedef std::chrono::monotonic_clock __clock;
	#endif
		typedef __clock::time_point TimePoint;
		typedef __clock::duration Duration;
		///
		template<typename _Rep, typename _Period>
		static inline constexpr std::chrono::duration<_Rep,_Period> getDuration(const std::chrono::duration<_Rep,_Period>& dura){return dura;	}
		template<typename _Clock, typename _Dur>
		static inline constexpr _Dur getDuration(const std::chrono::time_point<_Clock,_Dur>& tp){return tp.time_since_epoch();}
		///
		template<class _TClock = __clock>
		static constexpr inline typename _TClock::time_point Now(){return _TClock::now();}

		template<class _T = Duration>
		static constexpr inline _T Min(){return _T::min();}
		template<class _T = Duration>
		static constexpr inline _T Max(){return _T::max();}
		template<class _T = Duration>
		static constexpr inline _T Zero(){
			return static_cast<_T>(
					std::conditional<std::chrono::__is_duration<_T>::value,_T, typename _T::duration>::type::zero()
			);
		}
		template<class _Rst,class _T>
		static constexpr inline _Rst Cast(const _T& input){
			return _Rst(getDuration(input));
		};
		///
		typedef std::chrono::milliseconds MilliSeconds;
		typedef std::chrono::microseconds MicroSeconds;
		typedef std::chrono::nanoseconds NanoSeconds;
		typedef std::chrono::seconds Seconds;
		typedef std::chrono::minutes Minutes;
		typedef std::chrono::hours Hours;

		///
	#if __cplusplus >= 201103L
		constexpr Duration operator"" _ms	 	(uint64 n){return (Duration)MilliSeconds(n);}
		constexpr Duration operator"" _us	 	(uint64 n){return (Duration)MicroSeconds(n);}
		constexpr Duration operator"" _ns	 	(uint64 n){return (Duration)NanoSeconds (n);}
		constexpr Duration operator"" _s	 	(uint64 n){return (Duration)Seconds(n);}
		constexpr Duration operator"" _m	 	(uint64 n){return (Duration)Minutes(n);}
		constexpr Duration operator"" _h	 	(uint64 n){return (Duration)Hours(n);}
		constexpr Duration operator"" _second	(uint64 n){return (Duration)Seconds(n);}
		constexpr Duration operator"" _sec   	(uint64 n){return (Duration)Seconds(n);}
		constexpr Duration operator"" _min		(uint64 n){return (Duration)Minutes(n);}
		constexpr Duration operator"" _hour		(uint64 n){return (Duration)Hours(n);}
	#endif

		///
		template<class _This, class _Time>
		static inline int64 to(const _This &this_){return std::chrono::duration_cast<_Time>(getDuration(this_)).count();}
		template<class _This>static constexpr inline int64 toHours(const _This &this_) { return to<_This,Hours>(this_); }
		template<class _This>static constexpr inline int64 toMinutes(const _This &this_) { return to<_This,Minutes>(this_); }
		template<class _This>static constexpr inline int64 toSeconds(const _This &this_) { return to<_This,Seconds>(this_); }
		template<class _This>static constexpr inline int64 toMs(const _This &this_) { return to<_This,MilliSeconds>(this_); }
		template<class _This>static constexpr inline int64 toUs(const _This &this_) { return to<_This,MicroSeconds>(this_); }
		template<class _This>static constexpr inline int64 toNs(const _This &this_) { return to<_This,NanoSeconds >(this_); }

		template<class _This, class _Time>
		static inline _This from(int64 v) { return _This(_Time(v)); }
		template<class _This=Duration>static constexpr inline _This fromHours(int64 v) { return from<_This,Hours>(v); }
		template<class _This=Duration>static constexpr inline _This fromMinutes(int64 v) { return from<_This,Minutes>(v); }
		template<class _This=Duration>static constexpr inline _This fromSeconds(int64 v) { return from<_This,Seconds>(v); }
		template<class _This=Duration>static constexpr inline _This fromMs(int64 v) { return from<_This,MilliSeconds>(v); }
		template<class _This=Duration>static constexpr inline _This fromUs(int64 v) { return from<_This,MicroSeconds>(v); }
		template<class _This=Duration>static constexpr inline _This fromNs(int64 v) { return from<_This,NanoSeconds >(v); }


		///
		template<class _This>
		static constexpr inline struct timeval toTimeVal(const _This &this_) {
			return timeval{(time_t)toSeconds(this_),(time_t)(toUs(this_)%1000000)};
		}
		template<class _This>
		static constexpr inline _This fromTimeVal(const struct timeval tv) {
			return fromUs<_This>(tv.tv_sec*1000000ull+tv.tv_usec);
		}

		///
		template<class _This>
		static constexpr inline struct timespec toTimeSepc(const _This &this_) {
			auto sec = Seconds(this_);
			auto nsec= MicroSeconds(this_-sec);
			return timespec{sec.count(),nsec.count()};
		}
		template<class _This>
		static constexpr inline _This fromTimeSpec(const struct timespec tv) {
			return Seconds(tv.tv_sec)+MicroSeconds(tv.tv_nsec);
		}

		////
		typedef std::function<bool(TimePoint)> FuncTimePoint;
		typedef std::function<bool(Duration)> FuncDuration;
		////

		const TimePoint _timepoint_min =Min<TimePoint>();
		const TimePoint _timepoint_max =Max<TimePoint>();
		const TimePoint _timepoint_zero =Zero<TimePoint>();
		const Duration _duration_min =Min<Duration>();
		const Duration _duration_max =Max<Duration>();
		const Duration _duration_zero =Zero<Duration>();

		const Duration _timeout_nowait = Zero<Duration>();
		const Duration _timeout_forever = Max<Duration>();


		template<typename TRep, typename TPeriod>inline
		std::ostream&operator<<(std::ostream&co, const std::chrono::duration<TRep, TPeriod> &v) {
			return co << toMs(v) << "ms";
		}

		template<class TClock, class TDuration>inline
		std::ostream&operator<<(std::ostream&co, const std::chrono::time_point<TClock, TDuration> &v) {
			return co << v.time_since_epoch();
		};

		template<typename TRep0, typename TPeriod0,typename TRep1, typename TPeriod1>inline
		bool operator==(const std::chrono::time_point<TRep0, TPeriod0> &tp, const std::chrono::duration<TRep1, TPeriod1> &dura) {
			return tp.time_since_epoch() == dura;
		}

		template<typename TRep0, typename TPeriod0,typename TRep1, typename TPeriod1>inline
		bool operator==(const std::chrono::duration<TRep0, TPeriod0> &dura, const std::chrono::time_point<TRep1, TPeriod1> &tp) {
			return tp.time_since_epoch() == dura;
		}
	}
}

///for 
#define DurationCast std::chrono::duration_cast
#define TimePointCast std::chrono::time_point_cast
#define DurationToSec(v) toSeconds(v)//DurationCast< ::Seconds>(v).count()
#define DurationToMs(v) toMs(v)//DurationCast< ::MilliSeconds>(v).count()
#define DurationToUs(v) toUs(v)//DurationCast< ::MicroSeconds>(v).count()
#define DurationToNs(v) toNs(v)//DurationCast< ::MicroSeconds>(v).count()
#define TimePointToSec(v) toSeconds(v)//TimePointCast< ::Seconds>(v).time_since_epoch().count()
#define TimePointToMs(v) toMs(v)//TimePointCast< ::MilliSeconds>(v).time_since_epoch().count()
#define TimePointToUs(v) toUs(v)//TimePointCast< ::MicroSeconds>(v).time_since_epoch().count()
#define TimePointToNs(v) toNs(v)//TimePointCast< ::MicroSeconds>(v).time_since_epoch().count()
#define DurationFromSec(v) fromSeconds<::Duration>(v)//DurationCast< ::Duration>(Seconds(v))
#define DurationFromMs(v) fromMs< ::Duration>(v)//DurationCast< ::Duration>(MilliSeconds(v))
#define DurationFromUs(v) fromUs< ::Duration>(v)//DurationCast< ::Duration>(MicroSeconds(v))
#define DurationFromNs(v) fromNs< ::Duration>(v)//DurationCast< ::Duration>(MicroSeconds(v))
#define TimePointFromSec(v) formSeconds< ::TimePoint>(v)//::TimePoint(Seconds(v))
#define TimePointFromMs(v) fromMs< ::TimePoint>(v)//::TimePoint(MilliSeconds(v))
#define TimePointFromUs(v) fromUs< ::TimePoint>(v)//::TimePoint(MicroSeconds(v))
#define TimePointFromNs(v) fromNs< ::TimePoint>(v)//::TimePoint(MicroSeconds(v))

//#define _timepoint_min Min<TimePoint>()
//#define _timepoint_max Max<TimePoint>()
//#define _timepoint_zero Zero<TimePoint>()
//#define _duration_min Min<Duration>()
//#define _duration_max Max<Duration>()
//#define _duration_zero Zero<Duration>()


