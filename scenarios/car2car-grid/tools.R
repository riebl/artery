require(DBI)
require(RSQLite)
require(dplyr, warn.conflicts = FALSE)
require(stringr)
require(reshape2)
require(ggplot2)

extractNodeNumber <- function(module)
{
  as.integer(str_match(module, ".*node\\[([0-9]+)\\].*")[1,2])
}

buildVectorTable <- function(db, scaleexp = 12, vectors = NULL)
{
  vector <- tbl(db, 'vector')
  data <- tbl(db, 'vectordata')
  if (length(vectors) > 1) {
    vector <- vector %>% filter(vectorName %in% vectors)
  } else if (!is.null(vectors)) {
    vector <- vector %>% filter(vectorName == vectors)
  }

  vector %>% left_join(data, by = 'vectorId') %>%
    mutate(simtime = simtimeRaw / 10^scaleexp) %>%
    select(name = vectorName, module = moduleName, simtime, value)
}

buildNodeLookupTable <- function(vt)
{
  modules <- vt %>% distinct(module)  %>% collect()
  modules %>% rowwise() %>% mutate(node = extractNodeNumber(module))
}

mergeNodePositions <- function(vt)
{
    posx = vt %>% filter(name == 'posX:vector')
    posy = vt %>% filter(name == 'posY:vector')
    inner_join(posx, posy, by=c('simtime', 'module')) %>% select(module, simtime, x = value.x, y = value.y)
}

openOppDatabase <- function(filename)
{
  db <- DBI::dbConnect(RSQLite::SQLite(), filename)
  RSQLite::initExtension(db)
  DBI::dbSendQuery(db, 'PRAGMA temp_store = MEMORY;')
  return(db)
}

filterRegionOfInterest <- function(vt, pos, nodes)
{
  lhs <- vt %>% mutate(simtime10 = as.integer(simtime * 10)) %>% left_join(nodes, by = c("module"))
  rhs <- pos %>% left_join(nodes, by = c("module")) %>% mutate(simtime10 = as.integer(simtime * 10))
  lhs %>% left_join(rhs, by = c("simtime10", "node")) %>% filter(x > 50.0, x < 350.0, y > 50.0, y < 350.0) %>% select(name, module = module.x, simtime = simtime.x, value)
}

camReceptions <- function(vt)
{
  gendeltatime <- vt %>% filter(name == 'reception:vector(camGenerationDeltaTime)')
  stationid <- vt %>% filter(name == 'reception:vector(camStationId)')
  gendeltatime %>% inner_join(stationid, by=c('simtime', 'module')) %>% select(module, simtime, genDeltaTime = value.x, stationId = value.y)
}

camTransmissions <- function(vt)
{
  gendeltatime <- vt %>% filter(name == 'transmission:vector(camGenerationDeltaTime)')
  stationid <- vt %>% filter(name == 'transmission:vector(camStationId)')
  gendeltatime %>% inner_join(stationid, by=c('simtime', 'module')) %>% select(module, simtime, genDeltaTime = value.x, stationId = value.y)
}

camLatencies <- function(tx, rx)
{
  tx %>% left_join(rx, by=c("genDeltaTime", "stationId")) %>% mutate(latency = simtime.y - simtime.x)
}

summarizeLatencies <- function(dt, lut, nodes)
{
  dt <- dt %>% filter(!is.na(latency))
  by_nodes <- dt %>% left_join(lut, by=c('module.x' = 'module')) %>% rename(tx = node) %>% filter(tx %in% nodes) %>%
    group_by(tx) %>% summarize(min = min(latency), max = max(latency), mean = mean(latency))
  add_row(by_nodes, tx = 'all', min = min(dt$latency), max = max(dt$latency), mean = mean(dt$latency))
}

summarizeReceptions <- function(rx, lut, nodes)
{
  rx <- rx %>% mutate(simtime_sec = floor(simtime))
  by_nodes <- rx %>% group_by(simtime_sec, module) %>% summarize(total = n(), stations = n_distinct(stationId)) %>% left_join(lut, by='module') %>% filter(node %in% nodes)
  by_network <- rx %>% group_by(simtime_sec) %>% summarize(total = n(), stations = n_distinct(stationId))
  list(by_nodes = by_nodes, by_network = by_network)
}

plotLatencies <- function(sum_dt)
{
  ggplot(sum_dt, aes(x = factor(tx, levels = tx), y = mean)) + geom_errorbar(aes(ymin = min, ymax = max)) + geom_point() +
    labs(x = 'Transmitting station', y = 'Latency at receiving station (seconds)')
}

plotNetworkReceptions <- function(rx_net)
{
  rx_net_long <- melt(rx_net, measure.vars = c('total', 'stations'))
  var_levels <- c('total', 'stations')
  var_labels <- c('all', 'by distinct stations')
  ggplot(rx_net_long, aes(x = simtime_sec, y = value, colour = factor(variable, levels = var_levels, labels = var_labels))) + geom_step() +
    labs(x = 'Time (simulation seconds)', y = 'Received CAMs per second', colour = 'Aggregation')
}

plotTransmissionPattern <- function(tx)
{
  tx_pattern <- tx %>% mutate(simtime_cyclic = as.integer(simtime * 10) %% 50 / 10) %>% group_by(simtime_cyclic) %>% summarize(count = n())
  ggplot(tx_pattern, aes(x=simtime_cyclic, y=count)) + geom_step() + labs(x="Cyclic simulation time", y="Generated CAMs")
}

plotVehicleCount <- function(pos)
{
  all <- pos %>% group_by(simtime) %>% summarize(count = n())
  roi <- pos %>% filter(x > 50, x < 350, y > 50, y < 350) %>% group_by(simtime) %>% summarize(count = n())
  vehs <- left_join(all, roi, by=c('simtime')) %>% select(simtime, all = count.x, roi = count.y)
  vehs$roi[is.na(vehs$roi)] <- 0
  vehs_long <- melt(vehs, id.vars = c('simtime'))
  var_labels = c('all', 'in ROI')
  var_levels = c('all', 'roi')
  ggplot(vehs_long, aes(x = simtime, y = value, colour = factor(variable, labels = var_labels, levels = var_levels))) +
    geom_line() + labs(x = "Simulation time (seconds)", y = "Vehicle count", colour = "Vehicle selector")
}
