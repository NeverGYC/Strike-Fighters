#ifndef TIMER_H_
#define TIMER_H_


namespace game {

    // A class implementing a simple timer
    class Timer {
    public:
        // Constructor and destructor
        Timer(void);
        ~Timer();

        // Start the timer now: end time given in seconds
        void Start(float end_time);

        // Check if timer has finished
        bool Finished(void) const;

    private:
        double start_time_;  // When timer started
        double end_time_;    // When timer should end
        bool running_;       // Is timer currently running?

    }; // class Timer

} // namespace game

#endif // TIMER_H_