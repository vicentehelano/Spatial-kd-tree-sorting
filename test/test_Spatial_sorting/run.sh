#!/bin/bash
# Copyright (C) 2018 Célestin Marot
#
# This file is part of Spatial-kd-tree-sorting.
#
# This is a free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with hxt_SeqDel.  If not, see <http://www.gnu.org/licenses/>.
#
# See the COPYING file for the GNU General Public License.
#
# Author: Vicente Sobrinho (vicente.sobrinho@ufca.edu.br)

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
        ./bin/Release/test_Spatial_sorting --${method} --${data} ${size}000000 >> ${method}-${data}${size}M.txt 2>&1
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
