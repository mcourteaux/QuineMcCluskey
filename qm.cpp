#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>
#include <iostream>
#include <set>
#include <string.h>

struct f_output
{
    int n_values;
    int *values;
    int n_dontcares;
    int *dontcares;
};

struct algo_line
{
    int tag;
    std::vector<int> numbers;
    char *bits;
    bool checked;
};

struct algo_col
{
    int col_nr;
    std::vector<std::vector<algo_line> > blocks_by_numbits;
};

int create_tag(f_output *outs, int nOutput, int num)
{
    int tag = 0;
    for (int i = 0; i < nOutput; ++i)
    {
        for (int k = 0; k < outs[i].n_values; ++k)
        {
            if (outs[i].values[k] == num)
            {
                tag |= (1 << i);
                break;
            }
        }
        for (int k = 0; k < outs[i].n_dontcares; ++k)
        {
            if (outs[i].dontcares[k] == num)
            {
                tag |= (1 << i);
                break;
            }
        }
    }
    return tag;
}

int num_bits(int i)
{
    int n = 0;
    while (i)
    {
        n += (i & 1);
        i >>= 1;
    }
    return n;
}

bool matches(int number, char *bits, int nBits)
{
    if (number >> nBits) return false;
    for (int i = 0; i < nBits; ++i)
    {
        if (bits[i] == '-') continue;
        if (((number >> i) & 1) != (bits[i] == '1')) return false;
    }
    return true;
}

void algo(int nBits, std::vector<algo_col> &cols)
{
    algo_col &col = cols.back();
    algo_col new_col;
    new_col.col_nr = col.col_nr + 1;

    bool created = false;
    for (int i = 0; i < col.blocks_by_numbits.size() - 1; ++i)
    {
        int j = i + 1;
        std::vector<algo_line> new_group;
        for (int k = 0; k < col.blocks_by_numbits[i].size(); ++k)
        {
            for (int l = 0; l < col.blocks_by_numbits[j].size(); ++l)
            {
                algo_line &line1 = col.blocks_by_numbits[i][k];
                algo_line &line2 = col.blocks_by_numbits[j][l];

                if (line1.tag & line2.tag)
                {
                    int diff_bit_index = -1;
                    for (int b = 0; b < nBits; ++b)
                    {
                        if (line1.bits[b] != line2.bits[b])
                        {
                            if (line1.bits[b] == '-' || line2.bits[b] == '-')
                            {
                                diff_bit_index = -1;
                                break;
                            }
                            if (diff_bit_index == -1)
                            {
                                diff_bit_index = b;
                            } else
                            {
                                diff_bit_index = -1;
                                break;
                            }
                        }
                    }
                    if (diff_bit_index != -1)
                    {
                        /* Add it to the new group */
                        algo_line combined_line;
                        combined_line.bits = new char[nBits];
                        memcpy(combined_line.bits, line1.bits, nBits);
                        combined_line.bits[diff_bit_index] = '-';
                        combined_line.tag = line1.tag & line2.tag;
                        combined_line.checked = false;
                        combined_line.numbers = line1.numbers;
                        combined_line.numbers.insert(combined_line.numbers.end(), line2.numbers.begin(), line2.numbers.end());

                        /* Check if we should check off the originals */
                        if (combined_line.tag == line1.tag)
                        {
                            line1.checked = true;
                        }
                        if (combined_line.tag == line2.tag)
                        {
                            line2.checked = true;
                        }
                        created = true;
                        new_group.push_back(combined_line);
                    }
                }
            }
        }
        /* Add the group to the new col. */
        new_col.blocks_by_numbits.push_back(new_group);
    }
    if (created)
    {
        cols.push_back(new_col);

        /* Repeat algo */
        algo(nBits, cols);
    }
}

int main()
{
    /* Prompt input */
    int nBits;
    std::cin >> nBits;
    int nOutput;
    std::cin >> nOutput;
    f_output *outs = new f_output[nOutput];
    for (int i = 0; i < nOutput; ++i)
    {
        f_output &f = outs[i];
        std::cin >> f.n_values;
        f.values = new int[f.n_values];
        for (int k = 0; k < f.n_values; ++k)
        {
            std::cin >> f.values[k];
        }

        std::cin >> f.n_dontcares;
        f.dontcares = new int[f.n_dontcares];
        for (int k = 0; k < f.n_dontcares; ++k)
        {
            std::cin >> f.dontcares[k];
        }
    }

    /* Enlist all the recorded numbers */
    std::set<int> allNumbers;
    for (int i = 0; i < nOutput; ++i)
    {
        f_output &f = outs[i];
        for (int k = 0; k < f.n_values; ++k)
        {
            allNumbers.insert(f.values[k]);
        }
        for (int k = 0; k < f.n_dontcares; ++k)
        {
            allNumbers.insert(f.dontcares[k]);
        }
    }
    std::cout << "All numbers: " << allNumbers.size() << std::endl;

    /* Group them by number of bits */
    std::vector<std::vector<int> > bitGroup;
    for (int i = 0; i < nBits + 1; ++i)
        bitGroup.push_back(std::vector<int>());
    for (auto it = allNumbers.begin(); it != allNumbers.end(); ++it)
    {
        bitGroup[num_bits(*it)].push_back(*it);
    }

    /* Convert to algo_line */
    std::vector<algo_col> algo_cols;
    algo_col col0;
    col0.col_nr = 0;
    for (int i = 0; i <= nBits; ++i)
    {
        std::vector<algo_line> l;
        std::vector<int> nums_of_i_bits = bitGroup[i];
        for (int k = 0; k < nums_of_i_bits.size(); ++k)
        {
            algo_line line;
            int num = nums_of_i_bits[k];
            line.tag = create_tag(outs, nOutput, num);
            line.checked = false;
            line.numbers.push_back(num);
            line.bits = new char[nBits];
            for (int b = 0; b < nBits; ++b)
            {
                bool s = num & (1 << b);
                line.bits[b] = s ? '1' : '0';
            }

            l.push_back(line);
        }

        col0.blocks_by_numbits.push_back(l);
    }
    algo_cols.push_back(col0);
    algo(nBits, algo_cols);

    /* Output the table */
    for (int i = 0; i < algo_cols.size(); ++i)
    {
        std::cout << std::endl;
        std::cout << "==== Column " << i << " ====" << std::endl;
        algo_col &col = algo_cols[i];
        for (int k = 0; k < col.blocks_by_numbits.size(); ++k)
        {
            for (int l = 0; l < col.blocks_by_numbits[k].size(); ++l)
            {
                algo_line &line = col.blocks_by_numbits[k][l];
                for (int b = 0; b < nBits; ++b)
                {
                    std::cout << line.bits[nBits - b - 1];
                }
                std::cout << "   ";
                for (int b = 0; b < nOutput; ++b)
                {
                    std::cout << ((line.tag & (1 << b)) ? '1' : '0');
                }
                std::cout << "  ";
                std::cout << (line.checked ? "X" : " ");
                std::cout << "  ";
                for (int n = 0; n < line.numbers.size(); ++n)
                {
                    std::cout << line.numbers[n] << ", ";
                }
                std::cout << "\b\b  " << std::endl;
            }
            std::cout << std::endl;
        }
    }

    /* Find prime implicants */
    std::vector<algo_line> prime_implicants;
    for (auto it = algo_cols.begin(); it != algo_cols.end(); ++it)
    {
        algo_col &col = *it;
        for (int i = 0; i < col.blocks_by_numbits.size(); ++i)
        {
            std::vector<algo_line> &lines = col.blocks_by_numbits[i];
            for (int k = 0; k < lines.size(); ++k)
            {
                algo_line &line = lines[k];
                if (!line.checked)
                {
                    auto it = std::find_if(prime_implicants.begin(), prime_implicants.end(), [&](algo_line &li) { return memcmp(li.bits, line.bits, nBits) == 0; });
                    if (it == prime_implicants.end())
                    {
                        prime_implicants.push_back(line);
                    }
                }
            }
        }
    }

    /* Count number of care values */
    int num_care_values = 0;
    for (int i = 0; i < nOutput; ++i)
    {
        num_care_values += outs[i].n_values;
    }

    /* Construct table for min coverage */
    char **table = new char*[prime_implicants.size()];
    int *header = new int[num_care_values];
    for (int k = 0, c = 0; k < nOutput; ++k)
    {
        f_output &out = outs[k];
        for (int l = 0; l < out.n_values; ++l, ++c)
        {
            header[c] = out.values[l];
        }
    }
    for (int i = 0; i < prime_implicants.size(); ++i)
    {
        algo_line &line = prime_implicants[i];
        table[i] = new char[num_care_values];
        memset(table[i], ' ', num_care_values);
        int c = 0;
        for (int k = 0; k < nOutput; ++k)
        {
            f_output &out = outs[k];
            if (line.tag & (1 << k))
            {
                for (int l = 0; l < out.n_values; ++l, ++c)
                {
                    int number = out.values[l];
                    if (matches(number, line.bits, nBits))
                    {
                        table[i][c] = 'x';
                    }
                }
            } else
            {
                c += out.n_values;
            }
        }
    }

    /* Print table */
    /* Header */
    std::cout << "          ";
    for (int i = 0, f = 0, o = 0; i < num_care_values; ++i)
    {
        if (i - o >= outs[f].n_values)
        {
            std::cout << "    ";
            f++;
            o = i;
        }
        std::cout << std::setw(2) << header[i] << " ";
    }
    std::cout << std::endl;

    /* Table */
    for (int i = 0; i < prime_implicants.size(); ++i)
    {
        algo_line &pi = prime_implicants[i];
        std::stringstream ss;
        for (int b = 0; b < nBits; ++b)
        {
            ss << pi.bits[nBits - b - 1];
        }
        std::cout << std::left << std::setw(10) << ss.str() << std::right;

        /* Stars */
        for (int k = 0, f = 0, o = 0; k < num_care_values; ++k)
        {
            if (k - o >= outs[f].n_values)
            {
                std::cout << "    ";
                f++;
                o = k;
            }
            std::cout << std::setw(2) << table[i][k] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
