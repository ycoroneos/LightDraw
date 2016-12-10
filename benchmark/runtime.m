x=0:5000;
y_lidr = log10(3*x);
y_lidr_shadows = log10(3*x + x.^2);
y_forward_noshadows = log10(x+ x.^2);
y_forward_shadows = log10(x+ x.^2 + x.^2);


figure;
plot(x, y_lidr, x, y_lidr_shadows, x, y_forward_noshadows, x, y_forward_shadows, 'LineWidth', 1);
legend('lidr\_no\_shadows\_runtime', 'lidr\_shadows\_runtime', 'forward\_no\_shadows\_runtime', 'forward\_shadows\_runtime');
ylabel('log10 runtime');
xlabel('# lights + objects');