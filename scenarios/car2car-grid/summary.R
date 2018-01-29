vt_roi <- filterRegionOfInterest(vt, pos, lut)
rx_roi <- camReceptions(vt_roi)
tx_roi <- camTransmissions(vt_roi)
dt_roi <- camLatencies(tx_roi, rx_roi)

# Caution, SUMO vehicle "0" is node[1] etc.
stations <- seq(61, 3400, by=200)
sum_dt <- summarizeLatencies(dt_roi, lut, stations)
sum_rx <- summarizeReceptions(rx_roi, lut, stations)

plot_dt <- plotLatencies(sum_dt)
plot_net_rx <- plotNetworkReceptions(sum_rx$by_network)
