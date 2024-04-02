#!/bin/bash

runs="1 2 3"
sizes="1 10 20 30 35 40"
methods="hxt kdt"
datasets="axes cube cylinder disk planes paraboloid spiral saddle"
tag=`date +%d-%h-%Y-%H:%M`
to="vicente.sobrinho@ufca.edu.br"

for run in ${runs}; do
  echo "Rodada Nº "$run
  for size in ${sizes}; do
    for method in ${methods}; do
      echo "  Método: "${method}
      for data in ${datasets}; do
        echo "    | Processando o conjunto '"${data}"' com "${size}"M pontos"
        ./bin/Release/test_Spatial_sorting --${method} --${data}000000 ${size} >> ${method}-${data}${size}M.txt 2>&1
      done
    done
  done
done

echo "Preparando os dados gerados para envio..."
for method in ${methods}; do
  folder="test_Spatial_sorting"-${method}-${tag}
  mkdir -p ${folder}
  mv ${method}-*.txt ${folder}/
  zip ${folder}.zip ${folder}/*
  
  echo "  > Enviando os resultados do método " ${method}
  echo "That's all folks!" | mutt -s "${folder}" -a "${folder}".zip -- ${to}
done
echo "Feito!"
