#ifndef SEARCHER_HPP_
#define SEARCHER_HPP_

#include <utility>
#include <vector>
#include <queue>

#include "common.hpp"
#include "indexer.hpp"

namespace ir
{
  namespace searcher
  {

      typedef indexer::indexer::pos_iter pos_iter;
      typedef std::pair<pos_iter, pos_iter> pos_range;

      struct merge_iter
      {
          merge_iter(const pos_iter& iter, int from):
            iter(iter), from(from)
          { }

          pos_iter iter;
          int from;
      };

      bool operator < (const merge_iter& x, const merge_iter& y);
      bool operator > (const merge_iter& x, const merge_iter& y);

      std::vector<indexer::posting>
        merge_n(const std::vector<pos_range>& ranges);

      std::vector<indexer::posting>
        intersect_two(const pos_range& x_begin, const pos_range& x_end,
                      const pos_range& y_begin, const pos_range& y_end);

      std::vector<indexer::posting>
        intersect_n(const std::vector<pos_range>& ranges);

      template <typename T>
      void merge_n_impl(const std::vector<pos_range>& ranges, T& out)
      {
        std::priority_queue<merge_iter,
                            std::vector<merge_iter>,
                            std::greater<merge_iter> > pq;

        for(size_t i = 0; i < ranges.size(); i++) {
          if(ranges[i].first == ranges[i].second)
            continue;

          pq.push(merge_iter(ranges[i].first, i));
        }

        while(!pq.empty()) {
          auto p_pos = pq.top();
          pq.pop();

          auto& iter = p_pos.iter;
          int from = p_pos.from;

          out.push_back(*iter);
          iter++;
          if(iter != ranges[from].second)
            pq.push(merge_iter(iter, from));
        }
      }

      class searcher
      {
        public:

          searcher(const std::string& filename);

          std::vector<indexer::posting>
            handle_query(const std::vector<std::wstring>& terms) const;

        private:
          pos_range calc_postings(const std::wstring& term) const;

          indexer::indexer indexer_;
      };

  }
}

#endif
