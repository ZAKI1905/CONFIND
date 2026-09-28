// Seeded differential harness preserved from the independent review.
// The same source is compiled against (a) the vendored historical headers/archives and
// (b) the candidate headers/archives. It only uses APIs that exist in both:
// SetGrid, SetContVal, SetGridVals(GridVals_2D*), SetFunc + SetGridVals(Mode),
// GetContourSet, Cont2D::size/[]/GetVal/GetFound/ConvertToCurve2D.
// All sample values are computed here at -O0 with pow called through a volatile
// pointer, so any output difference originates inside the linked CONFIND library.
#include <Confind/ContourFinder.hpp>
#include <Zaki/Vector/DataSet.hpp>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

static double (*volatile POW)(double, double) = std::pow;
static uint64_t bits(double d) { uint64_t u; std::memcpy(&u, &d, 8); return u; }
static uint64_t H(uint64_t h, uint64_t w) { for (int k = 0; k < 8; ++k) { h ^= (w >> (8 * k)) & 0xff; h *= 1099511628211ULL; } return h; }

struct Rng { uint64_t s; double u() { s ^= s >> 12; s ^= s << 25; s ^= s >> 27; return double((s * 2685821657736338717ULL) >> 11) / 9007199254740992.0; } };

static std::function<double(double, double)> g_f;
static double tramp(double x, double y) { return g_f(x, y); }

static int DUMP_ID = -1;
static void Dump(FILE* out, const char* tag, int id, std::vector<CONFIND::Cont2D> cs) {
  if (id == DUMP_ID && std::string(tag) == "sampled") for (size_t c = 0; c < cs.size(); ++c) for (size_t i = 0; i < cs[c].size(); ++i) std::fprintf(out, "PT\t%zu\t%zu\t%a\t%a\t%.17g\t%.17g\n", c, i, cs[c][i].x, cs[c][i].y, cs[c][i].x, cs[c][i].y);
  for (size_t c = 0; c < cs.size(); ++c) {
    uint64_t hr = 1469598103934665603ULL, hc = hr; size_t curve = 0;
    for (size_t i = 0; i < cs[c].size(); ++i) { auto p = cs[c][i]; hr = H(H(H(hr, bits(p.x)), bits(p.y)), bits(p.z)); }
    if (cs[c].size()) {
      auto cv = cs[c].ConvertToCurve2D(); curve = cv.pts.size();
      for (auto& q : cv.pts) hc = H(H(hc, bits(q.x)), bits(q.y));
    }
    std::fprintf(out, "%d\t%s\t%zu\t%a\t%d\t%zu\t%016llx\t%zu\t%016llx\n", id, tag, c, cs[c].GetVal(), int(cs[c].GetFound()),
                 cs[c].size(), (unsigned long long)hr, curve, (unsigned long long)hc);
  }
}

int main(int argc, char** argv) {
  FILE* out = std::fopen(argv[1], "w");
  const int count = argc > 2 ? std::atoi(argv[2]) : 600;
  const int only_log = argc > 3 ? std::atoi(argv[3]) : 0;
  DUMP_ID = argc > 4 ? std::atoi(argv[4]) : -1;
  Rng rng{0xC0FFEE1234567ULL};
  for (int id = 0; id < count; ++id) {
    size_t nx = 1 + size_t(rng.u() * 40), ny = 1 + size_t(rng.u() * 40);
    int logmask = only_log == 1 ? 1 + int(rng.u() * 3) : int(rng.u() * 4);
    if (only_log == 2) logmask = 0;   // bit0: x log, bit1: y log
    double ax, bx, ay, by;
    if (logmask & 1) { double e0 = -3 + rng.u() * 18, e1 = e0 + 0.1 + rng.u() * 3; ax = POW(10, e0); bx = POW(10, e1); }
    else { ax = -5 + 10 * rng.u(); bx = ax + 0.01 + 10 * rng.u(); }
    if (logmask & 2) { double e0 = -3 + rng.u() * 18, e1 = e0 + 0.1 + rng.u() * 3; ay = POW(10, e0); by = POW(10, e1); }
    else { ay = -5 + 10 * rng.u(); by = ay + 0.01 + 10 * rng.u(); }
    Zaki::Math::Grid2D g{{{ax, bx}, nx, (logmask & 1) ? "Log" : "Linear"}, {{ay, by}, ny, (logmask & 2) ? "Log" : "Linear"}};
    // Smooth field in normalized coordinates (u,v in [0,1] on the log/linear sampling axis).
    const double lx0 = (logmask & 1) ? std::log10(ax) : ax, lx1 = (logmask & 1) ? std::log10(bx) : bx;
    const double ly0 = (logmask & 2) ? std::log10(ay) : ay, ly1 = (logmask & 2) ? std::log10(by) : by;
    const double c1 = rng.u(), c2 = rng.u(), w1 = 2 + 6 * rng.u(), w2 = 2 + 6 * rng.u(), ph = 6.28 * rng.u();
    std::function<double(double, double)> f = [=](double x, double y) {
      double u = (((logmask & 1) ? std::log10(x) : x) - lx0) / (lx1 - lx0);
      double v = (((logmask & 2) ? std::log10(y) : y) - ly0) / (ly1 - ly0);
      return std::sin(w1 * u + ph) * std::cos(w2 * v) + 0.7 * std::exp(-((u - c1) * (u - c1) + (v - c2) * (v - c2)) * 9);
    };
    std::vector<double> levels;
    int nl = 1 + int(rng.u() * 3);
    for (int l = 0; l < nl; ++l) levels.push_back(-0.8 + 1.8 * rng.u());
    // Sampled path, samples at x0 + i*dx in sampling space (same expression as SetDeltas).
    const double dx = (lx1 - lx0) / nx, dy = (ly1 - ly0) / ny;
    std::vector<double> iv, jv, zv;
    for (size_t j = 0; j <= ny; ++j) for (size_t i = 0; i <= nx; ++i) {
      double x = lx0 + i * dx, y = ly0 + j * dy;
      iv.push_back(i); jv.push_back(j);
      zv.push_back(f((logmask & 1) ? POW(10, x) : x, (logmask & 2) ? POW(10, y) : y));
    }
    Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{{"i", iv}, {"j", jv}, {"z", zv}});
    Zaki::Math::GridVals_2D gv(ds, 0, nx, 1, ny, 2);
    { CONFIND::ContourFinder c; c.SetGrid(g); c.SetContVal(levels); c.SetGridVals(&gv); Dump(out, "sampled", id, c.GetContourSet()); }
    g_f = f;
    { CONFIND::ContourFinder c; c.SetGrid(g); c.SetContVal(levels); c.SetFunc(&tramp); c.SetGridVals(CONFIND::ContourFinder::Fast); Dump(out, "fast", id, c.GetContourSet()); }
    { CONFIND::ContourFinder c; c.SetGrid(g); c.SetContVal(levels); c.SetFunc(&tramp); c.SetGridVals(CONFIND::ContourFinder::Normal); Dump(out, "normal", id, c.GetContourSet()); }
#ifdef CONFIND_MODERN
    for (size_t w : {1, 3, 8}) {
      CONFIND::ContourFinder c; c.SetGrid(g); c.SetContVal(levels);
      c.Evaluate([&](size_t) { return f; }, {w});
      Dump(out, ("evaluate_w" + std::to_string(w)).c_str(), id, c.GetContourSet());
    }
#endif
  }
  std::fclose(out);
  return 0;
}
