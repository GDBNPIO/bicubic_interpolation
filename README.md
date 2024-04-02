# bicubic_interpolation
basicly write a bicubic algorithm without function provided by OpenCV

1. The code makes use of two libraries: <iostream> and <opencv2/opencv.hpp>.
2. hfuction Function: This function calculates the weights for bicubic interpolation based on the given distance x. It uses different formulas for different distance intervals, as shown below:

$$
h(t) = 
\begin{cases} 
|t|^3 - 2|t|^2 + 1, & \text{if } |t| \leq 1 \\
-|t|^3+5|t|^2 - 8|t| + 4, & \text{if } 1 < |t| \leq 2 \\
0, & \text{if } |t| > 2
\end{cases}
$$

3. The MOZU bird photo input size: 378x283; output size: 1024x720.
4. I selected a 4x4 area in the same position of two resized images and printed their grayscale values, as shown in the illustration below. It can be found that there is a slight difference with the built-in function of OpenCV, which may be caused by a different approximation method, within a reasonable range.

<p align="center">
  <img src="https://github.com/GDBNPIO/bicubic_interpolation/assets/25226269/36e34557-4157-4908-b639-97d723a39e3c" alt="">
</p>
