use std::collections::VecDeque;
use std::sync::Mutex;

pub struct SensorQueues {
    pub p1: Mutex<VecDeque<u16>>,
    pub p2: Mutex<VecDeque<u16>>,
    pub temp: Mutex<VecDeque<u16>>,
}

impl SensorQueues {
    pub fn new() -> Self {
        SensorQueues {
            p1: Mutex::new(VecDeque::with_capacity(1000)),
            p2: Mutex::new(VecDeque::with_capacity(1000)),
            temp: Mutex::new(VecDeque::with_capacity(1000)),
        }
    }
}
