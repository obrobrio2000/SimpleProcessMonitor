<h1 align="center">SimpleProcessMonitor</h1>

<p align="center">
  <img width="100%" src="https://user-images.githubusercontent.com/62136803/190913216-1330e136-27f3-41e3-a853-23a298a8693e.png" alt="SimpleProcessMonitor: screenshot">
</p>

<p align="center">SimpleProcessMonitor is a lightweight program to monitor your processes (similiar to top/htop).<br><br><a href="https://gitlab.com/grisetti/sistemi_operativi_2021_22/-/blob/main/so_2021_22_projects.txt">Project</a> for the Course of <a href="https://gitlab.com/grisetti/sistemi_operativi_2021_22">"Operating Systems"</a>, held by <a href="https://sites.google.com/dis.uniroma1.it/grisetti/home">Prof. Giorgio Grisetti</a> in the 2nd semester of the 3rd year of the Bachelor's Degree Course in Computer and System Engineering (A.Y. 2021/2022) at Sapienza University of Rome.</p>

<p align="center">
  <a href="https://github.com/TUEngineers/SimpleProcessMonitor/blob/main/LICENSE">
    <img alt="License" src="https://img.shields.io/badge/License-MIT-yellow.svg">
  </a>
</p>

<h2 align="center">Usage</h2>
Outside the program (from the terminal):

```bash
./SimpleProcessMonitor [help | terminate <pid> | kill <pid> | suspend <pid> | resume <pid> | interval <time_ss> | sort <cpu|mem|pid>]
```

Inside the program:

```
monitor | back                            - Go back to monitoring processes
terminate                                 - Terminate process with pid
kill                                      - Kill process with pid
suspend                                   - Suspend process with pid
resume                                    - Resume process with pid
interval                                  - Change monitoring refresh interval
                                            Measured in seconds, only integers allowed
                                            Default is 3 seconds, minimum is 1 second
highlight                                 - Highlight process with pid
unhighlight                               - Unhighlight currently highlighted process
sort                                      - Sort processes by cpu, mem or pid
                                            Default is cpu
help                                      - Print help dialog
exit | quit                               - Exit program
```
