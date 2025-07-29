# Brief
The goal of this project was to develop a model-based controller for a DC motor whose specs I did not have. 

The tasks were:
- Choosing the components
- Wiring the circuit 
- Identifying the system
- Designing the controller using the identified model
- Implementing the controller in code. 

# Hardware
The parts which I used in this project were:
- STM32F103C8T6 Microcontroller
- ST-Link V2 Debugger
- 5V DC Motor
- MT6701 Hall Effect Encoder
- L293D Motor Driver
- HW-131 Breadboard Power Supply
- 9V Battery

The components are connected as shown in Figure 1. 

***figure 1: circuit***

# System Identification
To identify the system dynamics, I applied a PRBS (Pseudo-Random Binary Sequence) voltage input to the motor, then measured the output angular velocity.
There are two advantages: 
  1) PRBS is easier to implement on embedded hardware than a frequency sweep
  2) The frequency spectrum of the sequence is white noise, so all frequencies are excited with the same density.

After I collected the data, there were some clearly incorrect data points; these were removed by simply dropping the rows, and the input-output data was as shown in Figure 2.

<p align="center">
  <kbd>
    <img src="https://raw.githubusercontent.com/keatinl1/dc_motor_id_and_control/refs/heads/main/figs/prbs_input_output.png">
  </kbd>
</p>
<p align="center">
Figure 2: input output prbs.
</p>

An Autoregressive Moving Average with Exogenous Inputs (ARMAX) model was used in the MATLAB System Identification toolbox to make a discrete of the system and it achieved 74% accuracy.

The identified transfer function below has two poles and two zeros, which might seem like overkill for the simple system, but there was a steep drop in accuracy with simpler models (down to 53%), so the more complicated model is worth it.

$$
  G(z^{-1}) = \frac{94.83 z^{-1} + 193.3 z^{-2}}{1 - 0.5899 z^{-1} - 0.03679 z^{-2}}
$$

# Controller Design

After getting the system model, I wanted a fast settling time $\approx0.2s$ and as long as the PID controller was reasonably robust that was okay.

Using the settling time equation below and setting the crossover frequency ($\omega_c$) to 20rad/s, the MATLAB pidtune() app was used to design the controller with the gains:

$$
  T_s = \frac{4}{\omega_c}
$$

| Gain | Value       |
|------|-------------|
| Kp   | 0.0038      |
| Ki   | 0.0254      |
| Kd   | 6.58e-05    |

The following step response is produced in Figure 3.

<p align="center">
  <kbd>
    <img src="https://raw.githubusercontent.com/keatinl1/dc_motor_id_and_control/refs/heads/main/figs/step_response.png">
  </kbd>
</p>
<p align="center">
Figure 3: Step Response.
</p>

This controller is then applied to the system and the following result is achieved in Figure 4.

<p align="center">
  <kbd>
    <img src="https://raw.githubusercontent.com/keatinl1/dc_motor_id_and_control/refs/heads/main/figs/results1.png">
  </kbd>
</p>
<p align="center">
Figure 4: Results.
</p>

# Stability Analysis

