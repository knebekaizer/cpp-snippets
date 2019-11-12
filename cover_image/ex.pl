# ≈сть контейнер заданной ширины (допустим, 530 пикселей).
# ≈сть несколько (допустим, 15), картинок рандомной ширины (примерно от 100 до 200, но не об€зательно).
# Ќужно подобрать три из них, которые максимально плотно заполн€т контейнер встык (допустим, 180+180+170).

# http://ru.wikipedia.org/wiki/%C7%E0%E4%E0%F7%E0_%EE_%F0%E0%ED%F6%E5  «адача о ранце

#  1. —начала инициализируетс€ таблица динамического программировани€
#  2. ѕотом она заполн€етс€ (решение - в правом нижнем углу)
#  3. ѕотом вывод€тс€ составл€ющие решени€ 

# ѕодн€л тестовый стенд: http://a.permlug.org/

use warnings;
use strict;

use List::Util 'max';

sub way {
    my ($matr, $variant_arr, $variant_num, $local_width_int) = @_;

    my @params = ($matr, $variant_arr);
    my $cur_variant_width_int = $matr->[$variant_num][$local_width_int];
    my $prev_variant_width_int = $matr->[$variant_num-1][$local_width_int];

    return if $variant_num==1 and !$cur_variant_width_int;

    my @result;
    if ($variant_num == 1 or $cur_variant_width_int!=$prev_variant_width_int) {
        @result = way(@params, $variant_num, $local_width_int-$variant_arr->[$variant_num-1]);
        push @result, $variant_arr->[$variant_num-1];
    }
    else {
        @result = way(@params, $variant_num-1, $local_width_int);
    }

    return @result;
}

sub matr_prepare {
    my $width_int = shift;
    my @variant_arr = @_;
    my @matr;

    for my $variant_num (1..@variant_arr) {
        $matr[$variant_num][0] = 0;
    };

    for my $local_width_int (0..$width_int) {
        if ($local_width_int < $variant_arr[0]) {
            $matr[1][$local_width_int] = 0;
        }
        else {
            $matr[1][$local_width_int] = $variant_arr[0];
        }
    }

    for my $variant_num (2..@variant_arr) {
        for my $local_width_int (1..$width_int ) {
            my $result;

            my $prev_variant_num  = $variant_num-1;
            my $variant_width_int = $variant_arr[$prev_variant_num];
            my $width_delta_int   = $local_width_int-$variant_width_int;

            $matr[$variant_num][$local_width_int] = $width_delta_int>=0 ?
                max(
                    $matr[$prev_variant_num][$local_width_int],

$matr[$prev_variant_num][$width_delta_int]+$variant_width_int
                )
                :
                $matr[$prev_variant_num][$local_width_int]
            ;
        }
    }
    return @matr;
}

my $width_int   = 530;
my @variant_arr = map { int($_) } (
        101, 101, 111, 111, 121,
        121, 130, 141, 151, 160,
          1,   2,   3,   4,   5
);
my @matr   = matr_prepare($width_int => @variant_arr);
my @result = way(\@matr, \@variant_arr, scalar(@variant_arr), $width_int);

my $sum=0; $sum+= $_ for @result;
print join('+', sort { $b <=> $a } @result), ' = ', $sum, $/;

