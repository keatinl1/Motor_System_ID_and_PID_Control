## Brief Summary
The goal of this project was to develop a model-based controller for a DC motor whose specs I did not have. 

0. [Hardware](#hardware)
1. [System Identification](#system-identification)
2. [Controller Design](#controller-design)
3. [Stability Analysis](#stability-analysis)
4. [Results](#results)

$~~~~~~~~~~$

## Hardware
The parts which I used in this project are listed below; they are connected as shown in Figure 1. 
- STM32F103C8T6 Microcontroller
- ST-Link V2 Debugger
- 5V DC Motor
- MT6701 Hall Effect Encoder
- L293D Motor Driver
- HW-131 Breadboard Power Supply
- 9V Battery

***figure 1: circuit***

$~~~~~~~~~~$

## System Identification
To identify the system dynamics, I applied a PRBS (Pseudo-Random Binary Sequence) voltage input to the motor, then measured the output angular velocity. There are two advantages to this method: 
  1) PRBS is simpler and faster to implement on embedded hardware than a frequency sweep
  2) The frequency spectrum of the random sequence is white noise, so all frequencies are excited with the same density.

After I collected the data, there were some clearly incorrect data points (outliers) which were removed by simply dropping the rows. The clean input-output data is shown in Figure 2.

<p align="center">
  <kbd>
    <img src="https://raw.githubusercontent.com/keatinl1/dc_motor_id_and_control/refs/heads/main/figs/prbs_input_output.png">
  </kbd>
</p>
<p align="center">
Figure 2: PRBS input-output
</p>

An Autoregressive Moving Average with Exogenous Inputs (ARMAX) model was used in the MATLAB System Identification toolbox to make a discrete model of the system. A predictive accuracy of 74% was achieved.

The identified transfer function below has two poles and two zeros, which might seem like overkill for such a simple system, but simpler models had a steep drop in accuracy (down to 53%).

$$
  G(z^{-1}) = \frac{94.83 z^{-1} + 193.3 z^{-2}}{1 - 0.5899 z^{-1} - 0.03679 z^{-2}}
$$

$~~~~~~~~~~$

## Controller Design

The control requirements were a fast settling time, $\approx0.2s$, and as long as the PID controller was reasonably robust, that was acceptable.

Using the settling time equation below and setting the crossover frequency ($\omega_c$) to 20rad/s

$$
  T_s = \frac{4}{\omega_c}
$$

The MATLAB pidtune() app was used to design the controller, and the gains in the table below were found.
<p style="text-align: center;"><strong>Table 1: PID Gains</strong></p>

<div style="display: flex; justify-content: center;">
  <table>
    <tr><th>Gain</th><th>Value</th></tr>
    <tr><td>Kp</td><td>0.0038</td></tr>
    <tr><td>Ki</td><td>0.0254</td></tr>
    <tr><td>Kd</td><td>6.58e-05</td></tr>
  </table>
</div>

When the PID controller is applied to the model, the step response in Figure 3 is produced.
<p align="center">
  <kbd>
    <img src="https://raw.githubusercontent.com/keatinl1/dc_motor_id_and_control/refs/heads/main/figs/step_response.png">
  </kbd>
</p>
<p align="center">
Figure 3: Model Step Response.
</p>

$~~~~~~~~~~$

## Stability Analysis


## Results
The controller is applied to the system, and the result in Figure 4 is achieved.
<p align="center">
  <kbd>
    <img src="https://raw.githubusercontent.com/keatinl1/dc_motor_id_and_control/refs/heads/main/figs/results1.png">
  </kbd>
</p>
<p align="center">
Figure 4: Results.
</p>
