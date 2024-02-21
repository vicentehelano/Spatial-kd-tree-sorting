# Spatial-kd-tree-sorting
.
.
A maioria dos fenômenos naturais que conhecemos pode ser expresso hoje em dia por equações diferenciais parciais. Quando definidas sobre domínios e condições iniciais e de contorno simples, muitas vezes é possível calcular a solução exata. Mas a verdade é que sempre recorremos a soluções aproximadas toda vez que lidamos com simulações realistas. O método dos elementos finitos é a técnica mais utilizada para obter soluções aproximadas de equações diferenciais parciais. A ideia fundamental desse método é subdividir o domínio em pedaços menores sobre os quais são calculadas aproximações locais que, posteriormente, são combinadas para montar a solução global do problema. Esses "pedaços", denominados elementos, geralmente possuem a forma de triângulos e quadriláteros em 2D e tetraedros, cubos e pirâmides em 3D. Juntos, eles formam o que conhecemos como malha.

Implementações robustas do método dos elementos finitos requerem malhas de alta qualidade. O processo de geração dessas malhas é parte essencial de toda simulação pelo método dos elementos finitos. Há diversos métodos de geração de malha, mas aqui destacamos aqueles baseados na triangulação de Delaunay. O desempenho desses métodos depende do algoritmo utilizado para construir triangulações de Delaunay. Com as arquiteturas paralelas dominando a indústria de microprocessadores, próximos de termos em casa dispositivos eletrônicos equipados com centenas de núcleos, o interesse em algoritmos paralelos que construam triangulações de Delaunay é crescente. Infelizmente, nem todos os algoritmos paralelos desenvolvidos até o presente momento poderão aproveitar de toda essa potência, pois muitos deles sofrem com problemas de escalabilidade ([Marot, 2020](https://dial.uclouvain.be/pr/boreal/en/object/boreal%3A240626)). Resultados encorajadores foram obtidos recentemente por [Marot et al. (2019)](https://dial.uclouvain.be/pr/boreal/object/boreal:213759). Eles conseguiram gerar 3 bilhões de tetraedros em menos de um minuto e os gráficos de speedups sugerem um elevado grau de escalabilidade. Além disso, o código sequencial deles é cerca de 3 vezes mais rápido do que a CGAL, a mais sofisticada biblioteca de geometria computacional dos dias atuais.

Um componente crucial do algoritmo de [Marot et al. (2019)](https://dial.uclouvain.be/pr/boreal/object/boreal:213759) é a ordem na qual os pontos são inseridos na triangulação. De fato, como recentemente investigado por [Gonzaga e Oliveira (2018)](http://dx.doi.org/10.1007/s40314-016-0358-0), a ordem de inserção dos pontos pode alterar drasticamente o desempenho dos algoritmos de construção incremental de Delaunay. [Gonzaga e Oliveira (2018)](http://dx.doi.org/10.1007/s40314-016-0358-0) concluíram que ordenar os pontos segundo um tipo especial de $kd$-tree ([Liu et al., 2013](https://doi.org/10.1007/s10409-013-0001-x)) é mais eficiente do que usar a curva de Hilbert, contrariando os argumentos de [Marot et al. (2019)](https://dial.uclouvain.be/pr/boreal/object/boreal:213759).

## Objetivos

Neste contexto, o objetivo deste projeto é investigar se a implementação de [Marot et al. (2019)](https://dial.uclouvain.be/pr/boreal/object/boreal:213759) se beneficiará da inserção segundo uma $kd$-tree conforme argumentado por [Gonzaga e Oliveira (2018)](http://dx.doi.org/10.1007/s40314-016-0358-0). Mais especificamente, pretende-se:

- Implementar o esquema de ordenação de pontos via $kd$-trees de [Liu et al. (2013)](https://doi.org/10.1007/s10409-013-0001-x).
- Comparar o desempenho da implementação sequencial de [Marot et al. (2019)](https://dial.uclouvain.be/pr/boreal/object/boreal:213759) usando a curva de Hilbert e o esquema de [Liu et al. (2013)](https://doi.org/10.1007/s10409-013-0001-x).

## Metodologia

A pesquisa será iniciada com o estudo da triangulação de Delaunay e seus algoritmos clássicos de construção. Em seguida, deverão ser estudados os artigos de  
[Liu et al. (2013)](https://doi.org/10.1007/s10409-013-0001-x) e [Gonzaga e Oliveira (2018)](http://dx.doi.org/10.1007/s40314-016-0358-0).

O código sequencial desenvolvido por [Marot et al. (2019)](https://dial.uclouvain.be/pr/boreal/object/boreal:213759) está disponível em: 

[https://git.immc.ucl.ac.be/hextreme/hxt_seqdel](https://git.immc.ucl.ac.be/hextreme/hxt_seqdel)

Serão realizados experimentos para verificar os efeitos da ordenação via $kd$-tree no desempenho do código acima disponibilizado.
