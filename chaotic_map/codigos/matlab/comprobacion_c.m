clear; close all; clc;

data = load("../c_codes/salida.txt");
xn = data(:,1);
yn = data(:,2);

plot(xn,yn,'.r','MarkerSize',0.5); 
grid on; grid minor;
xlabel("xn"); 
ylabel("yn"); 
axis equal;