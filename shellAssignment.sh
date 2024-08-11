#!/bin/bash
itemFile=$1
userFile=$2
dataFile=$3

echo "
User Name:임정섭
Student Number: 12201838
[ MENU ]
1. Get the data of the movie identified by a specific
'movie id' from u.item
2. Get the data of action genre movies from '
u.item
3. Get the average 'rating’ of the movie identified by
specific 'movie id' from ' u.data
4. Delete the ‘IMDb URL’ from ‘
u.item
5. Get the data about users from '
u.user
6. Modify the format of 'release date' in '
u.item
7. Get the data of movies rated by a specific 'user id'
from ' u.data
8. Get the average 'rating' of movies rated by users with
'age' between 20 and 29 and 'occupation' as '
9. Exit
--------------------------"

answer=0

while [ ${answer} ]
do
	echo  #줄바꿈 출력용
	read -p "Enter your choice [ 1-9 ] " userAnswer

	answer=${userAnswer}

	case ${userAnswer} in

		1)
		echo #줄바꿈 출력용
		read -p "Please enter the 'movie id’(1~1682): " movieId_1
		echo
		cat ${itemFile} | awk -F'|' -v id_1=${movieId_1} '$1 == id_1 {print $0}'
		;;
		
		2)
		echo 
		read -p "Do you want to get the data of ‘action’ genre movies from ' u.item ’?(y/n): " ans2
		#n으로 대답했을 땐 무엇을 해야하는가? - 동영상에서도 언급은 없으므로 그냥 Enter your choice 부분을 재출력하자.
		echo
		if [ ${ans2} == 'y' ]
		then
			#장르가 액션인걸 판단 : 7번째 필드가 1이면 Action 장르이다.
			#우선 액션장르인 모든 행을 뽑고, 파이프를 통해 소트 후 헤드하자. 
			cat ${itemFile} | awk -F '|' '$7 == 1 {print $1, " ", $2}' | sort -k1 -n | head -n 10
		fi
		;;

		3)
		echo 
		read -p "Please enter the 'movie id’(1~1682): " movieId_3
		echo
		#우선 u.data에서 아이디로 해당 영화에 대한 평점들을 모두 구하자.
		#행 수를 알기 위해 행마다 세주자.
		cat ${dataFile} | awk -v id_3=${movieId_3} '$2 == id_3 {print $3}' | awk -v id_3=${movieId_3} '{sum += $1; rowNumber += 1} END{print "average rating of", id_3, ":", sum/rowNumber}' #print는 기본구분자가 스페이스바하나인듯
		;;

		4)
		echo 
		#생각해보니 그냥 awk만 사용하여 5번째(db url)필드만 제거하고 출력하는것이 편리할 것 같다.
		read -p "Do you want to delete the IMDb URL’ from ‘ u.item ’?(y/n): " ans4
		echo
		if [ ${ans4} == 'y' ]
		then
			cat ${itemFile} | awk -F '|' 'NR <= 10 {printf("%s|%s|%s|%s||%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $1, $2, $3, $4, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20, $21, $22, $23, $24)}'
		fi
		;;

		5)
		echo 
		read -p "Do you want to get the data about usersfrom ‘ u.user ’?(y/n): " ans5
		echo
		if [ ${ans5} == 'y' ]
		then
			#M을 male로 F를 female로 먼저 바꿔주자 (by word boundary 지정 : \b \b)
			#그룹개념을 사용하여 나눠진 것들을 잡자, 정규표현식 쓸 때 |도 포함되어있으므로 없애줘야
			#마찬가지로 user파일의 끝에 있는 zipcode도 불필요하니 지워주자
			#\([^|]*\)로 |가 아닌 문자들을 그룹화, 괄호를 이스케이프처리
			#강의노트 6번 마지막에서 두번째페이지 예제참고
			cat ${userFile} | head -n 10 | sed -e 's/\bM\b/male/g' -e 's/\bF\b/female/g' -e 's/\(^[0-9]*\)|\([0-9]*\)|\([^|]*\)|\([^|]*\)|[0-9]*/user \1 is \2 years old \3 \4/g'  
		fi
		;;

		6)
		echo
		read -p "Do you want to Modify the format of ‘release data’ in ‘ u.item ’?(y/n): " ans6
		echo
		#Jan, Feb등 영단어표현을 숫자로 바꿔주는 작업이 필요하고(첫번째sed), 바꾼후에는 그대로 가져다 쓰면 될것이다(두번째sed)
		#20-Sep-1996 : 이런형식의 문자열을 정규표현식으로 나타내어 애만 교체해주면 된다. "숫자2자리-영문3자리-숫자4자리"를 -> 20-09-1996 : "숫자2자리-숫자2자리-숫자4자리"로 바꾼다.
		if [ ${ans6} == 'y' ]
		then
			cat ${itemFile} | tail -n 10 | sed -e 's/Jan/01/' -e 's/Feb/02/' -e 's/Mar/03/' -e 's/Apr/04/' -e 's/May/05/' -e 's/Jun/06/' -e 's/Jul/07/' -e 's/Aug/08/' -e 's/Sep/09/' -e 's/Oct/10/' -e 's/Nov/11/' -e 's/Dec/12/' |
			sed -E 's/([0-9]{2})-([0-9]{2})-([0-9]{4})/\3\2\1/g'		# {number}이라는 확장 정규표현식을 사용하기 위하여 -E옵션 사용, 확장정규표현식을 사용하면 소괄호를 이스케이프 처리 안해도 된다고 한다.
		fi
		;;

		7)
		echo
		read -p "Please enter the ‘user id’(1~943): " userId_7
		echo
		#윗부분출력(영화아이디)
		##
		#마지막 출력에선 |를 미출력해야하므로 전체행의수를 알아야할 필요가 있음. 
		# lastRowNumber=$(cat ${dataFile} | awk -v uid_7_tmp=${userId_7} '$1 == uid_7_tmp {count++} END{print count}')
		# cat ${dataFile} | awk -v uid_7=${userId_7} -v lastRowNum=${lastRowNumber} '$1 == uid_7 {if(NR<lastRowNum)printf("%s|", $2); else{printf("%s",  $2);}}'
		##
		# 위 해결 방식으로는, 문법의 이유인진 모르겠는데 작동을 안한다. 그냥 sed로 하면 될듯, 라인 끝을 $로 표현
		cat ${dataFile} | sort -k2 -n | awk -v uid_7=${userId_7} '$1 == uid_7 {printf("%s|",$2)}' | sed 's/|$//' #리디렉션을 쓰면 sed적용한 표준출력이 파일로 가버리므로 똑같은 명령어 한번 더 쳐야할듯. #####윗부분출력용
		cat ${dataFile} | sort -k2 -n | awk -v uid_7=${userId_7} '$1 == uid_7 {print $2}' | head -n 10 > targetMovIdAndMovName_12201838	#####파일에 기록용, 기록할땐 필요한 10행만 기록하면 됨.
		
		echo
		echo

		#아랫부분출력(영화아이디와 제목)
		##
		#위에서 출력한 영화아이디를 그대로 활용하여, 이번엔 itemFile에서 awk하면 된다. -> 바로 아래처럼 하나의 커맨드로는 안되는 모양이다.
		#cat ${dataFile} | sort -k2 -n | awk -v uid_7=${userId_7} '$1 == uid_7 {print $1 $2}' | awk '$2 == $1 {print $1 $2}' ${itemFile} #마지막 awk에서 $1은 첫번째 awk에서의 $2, 즉 영화아이디
		##
		#그냥 파일에다가 기록해놓고 반복문을 돌리자. 120번 라인 생성
		while read movieId_7
		do
			#이미 targetMovIdAndMovName_12201838에 movieId가 정렬되어있기에 여기서 별도로 정렬은 안해줘도 됨. movieId_7이 그대로 읽어줌.
			cat ${itemFile} | awk -F'|' -v mid_7=${movieId_7} '$1 == mid_7 {print $1,"|",$2}'
 		done < targetMovIdAndMovName_12201838
		;;
		
		8)
		echo
		read -p "Do you want to get the average 'rating' of movies rated by users with 'age' between 20 and 29 and 'occupation' as 'programmer'?(y/n): " ans8
		echo
		if [ ${ans8} == 'y' ]
		then
			#20대유저가 평가하지 않은 영화는 출력하지 않음
			#즉 SQL의 inner 조인같은 개념이다. 유저를 찾고, 유저id를 통해 조인을 걸어줄 것

			#찾으려는 유저의 유저아이디를 뽑아냄 -> targetUser_12201838에 저장
			cat ${userFile} | awk -F'|' '($2 >= 20 && $2 <= 29) && $4 == "programmer" {print $1}' > targetUser_12201838	#혹시라도 파일명이 겹칠수도 있으니까 학번넣음	
			
			#(1)data에서 유저아이디를 바탕으로 유저아이디, 영화아이디, 평점을 뽑아냄 -> targetData_12201838에 저장 (매 번 append하므로 >>사용)
			#append개념으로 실행할것이므로, 항상 while루프를 돌기전 해당 파일을 빈 파일로 초기화해준다.
			truncate -s 0 targetData_12201838 
			while IFS='' read -r userId_8 
			do
				cat ${dataFile} | awk -v user_8=${userId_8} '$1 == user_8 {printf("%s|%s|%s\n",$1,$2,$3)}' >> targetData_12201838	#uid|mvid|score
			done < targetUser_12201838

			#맵으로 구하기 보다는, awk명령어로 평균을 구하는 것이 나을것 같다.
			cat targetData_12201838 | awk -F'|' '{sum[$2]+=$3; count[$2]++} END{for (i in sum) print i, sum[i]/count[i]}' | sort -n -k1

			#######아래는 실패한 map으로 평균구하는 코드
			# 평균을 구해야하므로 두 개의 맵 사용 1.sumsMap(영화별 평점 총합) 2.countsMap(영화별 평가 개수) , 두 맵의 키는 모두 movieId이다.
			# declare -A sumsMap
			# declare -A countsMap
			# # targetData_12201838에서 영화별 평균을 구하면 된다.
			# while IFS='|' read -r uid mvid scr
			# do
			# 	sumsMap+=([$mvid]=${sumsMap[$mvid]}+scr)
			# 	countsMap+=([$mvid]=${countsMap[$mvid]} + 1)
			# 	# sumsMap[$mvid]=$((sumsMap[$mvid] + scr))
			# 	# countsMap[$mvid]=$((countsMap[$mvid] + 1))
			# done < targetData_12201838	
			# #평균을 출력
			# for movieId_8 in "${!sumsMap[@]}"	
			# do
			# 	average=$(echo "scale=2; ${sumsMap[$movieId_8]} / ${countsMap[$movieId_8]}" | bc) #소수점지정필요,scale사용
			# 	echo "MovieID: $movieId_8, Average Score: $average"
			# done
		fi
		;;

		
		
		9)
		echo "Bye!"
		break
	esac
	# 리드미 : 스크립트의 사용법 및 구현내용을 hwp or pdf - 주석을 옮기면될듯? 
done
