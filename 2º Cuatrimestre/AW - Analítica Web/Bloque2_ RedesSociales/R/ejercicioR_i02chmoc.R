
# Leer el conjunto de datos del club de karate

nodes <- read.csv("nodosCarlos.csv", header = T)
links <- read.csv("enlacesCarlos.csv", header = T)

#Crear una red a partir de dichos datos (igraph)
net <- graph.data.frame(d=links,vertices=nodes,directed= F)

#Comprobar que el objeto net es de tipo igraph
class(net)

#Ver un resumen del tipo de dato
summary(net)

#Ver las primeras líneas de nodos y enlaces
head(nodes)
head(links)

#Acceder a las propiedades de los nodos y enlaces dentro del objeto net
V(net)
E(net)

#Acceder al campo label en los nodos y el campo weight en las aristas
table(V(net)$label)
table(E(net)$Weight)

plot(net)

#Utilizar las características de plot en igraph para optimizar el grafo
#vertex.color: color del nodo
#vertex.frame.color: color del borde del nodo
#vertex.label: etiquetas de los nodos
#vertex.label.cex: tamaño de letra de las etiquetas de los nodos
#...
#?igraph.plotting
plot(net, edge.arrow.size=.2, edge.curved=0,
     vertex.color="gray", vertex.frame.color="#555555",
     vertex.label=V(net)$label, vertex.label.color="black",
     vertex.label.cex=.5)

#Solamente mostrar las etiquetas sin nodos
plot(net, edge.arrow.size=.2, edge.curved=0,
     vertex.color="gray", vertex.frame.color="#555555",
     vertex.label=V(net)$label, vertex.label.color="black",
     vertex.label.cex=.5,vertex.shape="none")


#Utilizar layout para mostrar los nodos

#El layout aleatorio
plot(net, edge.arrow.size=.2, edge.curved=0,
     vertex.color="gray", vertex.frame.color="#555555",
     vertex.label=V(net)$label, vertex.label.color="black",
     vertex.label.cex=.5, vertex.size=5, layout=layout_randomly)

#Fruchterman-Reingold es uno de los layout basados en fuerza más utilizados
plot(net, edge.arrow.size=.2, edge.arrow.width=0.6, edge.curved=0,
     vertex.color="gray", vertex.frame.color="#555555",
     vertex.label=V(net)$label, vertex.label.color="black",
     vertex.label.cex=.5, vertex.size=5, layout=layout.fruchterman.reingold)



#Medidas generales de red: diámetro, densidad
diameter(net)
edge_density(net)

#Medidas generales de nodo: grado, intermediación

#Grado (se incluye como propiedad del nodo)
nodes$degree.total <- degree(net, v=V(net), mode="all")
nodes$degree.in <- degree(net,v=V(net), mode="in")
nodes$degree.out <- degree(net, v=V(net), mode = "out")

#Mostrar los 10 nodos con mejor de grado total
head(nodes[order(nodes$degree.total, decreasing= TRUE),], n=10L)

#Incorpor la información dentro del grafo
V(net)$outdegree <- degree(net, mode="out")
V(net)$indegree <- degree(net, mode="in")
V(net)$degree <- degree(net, mode="all")

#Intermediación
nodes$betweenness <- betweenness(net, v=V(net), directed=F,weights=NA)
V(net)$betweenness <- betweenness(net, directed=F,weights=NA)

#Dibujar el grafo con el tamaño del nodo en función del grado total
plot(net, edge.arrow.size=.2, edge.arrow.width=0.6, edge.curved=0,
     vertex.color="gray", vertex.frame.color="#555555",
     vertex.label=V(net)$label, vertex.label.color="black",
     vertex.label.cex=.5, vertex.size=V(net)$degree, layout=layout.fruchterman.reingold)

#Dibujar el grafo con el grosor de la arista en función del peso
plot(net, edge.arrow.size=.2, edge.arrow.width=0.6, edge.curved=0,
     vertex.color="gray", vertex.frame.color="#555555",
     vertex.label=V(net)$label, vertex.label.color="black",
     vertex.label.cex=.5, vertex.size=V(net)$degree, layout=layout.fruchterman.reingold,
     edge.width = E(net)$weight/3)

##############################
#Calcular subgrupos
##############################

###############################
# Cliques
##############################

# Contar cliques de tamaño mayor o igual a 3
count_max_cliques(net.sym, min = 3)

# Obtener el tamaño del clique más grande
clique_num(net.sym)

# Obtener los cliques de mayor tamaño
largest_cliques(net.sym)

###############################
# k-cores 
##############################

# Calcular el coreness de cada nodo
kc <- coreness(net)

# Obtener la distribución de nodos por k-core
table_kc <- table(kc)

# Obtener el número total de k-cores y el valor máximo
num_kcores <- length(table_kc)
max_k <- max(kc)
max_k_count <- table_kc[as.character(max_k)]

# Imprimir los resultados en el formato solicitado
cat("Hay", num_kcores, "k-cores en la red.", min(kc), " y ", max_k, "\n")
cat("Número de nodos en cada k-core:\n")

for (k in sort(unique(kc))) 
{
  cat("- K-core", k, ":", table_kc[as.character(k)], "nodos\n")
}

cat("k-core con el valor más alto: ", max_k, " con", max_k_count, "elementos.\n")

